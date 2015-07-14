/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2014 Oliver Groß.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <http://www.gnu.org/licenses/>.
 */

#ifndef OSMPBF_PARSEHELPERS_H
#define OSMPBF_PARSEHELPERS_H

#include <osmpbf/osmfilein.h>
#include <osmpbf/primitiveblockinputadaptor.h>

#include <mutex>
#include <atomic>
#include <thread>
#include <type_traits>

#if defined(_OPENMP)
#include <omp.h>
#endif

namespace osmpbf
{

namespace detail {
	template<typename T_PBI_PROCESSOR, typename T_RETURN_TYPE>
	struct PbiProcessor {
		inline static bool process(T_PBI_PROCESSOR & processor, osmpbf::PrimitiveBlockInputAdaptor & pbi) {
			return processor(pbi);
		}
	};
	
	template<typename T_PBI_PROCESSOR>
	struct PbiProcessor<T_PBI_PROCESSOR, void> {
		inline static bool process(T_PBI_PROCESSOR & processor, osmpbf::PrimitiveBlockInputAdaptor & pbi) {
			processor(pbi);
			return true;
		}
	};
	
	template<typename T>
	struct ProcessorPtr {
		static T * ptr(T & t) { return &t; }
	};

	template<typename T>
	struct ProcessorPtr<T*> {
		static T* ptr(T * t) { return t; }
	};
	
}

///@processor (osmpbf::PrimitiveBlockInputAdaptor & pbi)
template<typename TPBI_Processor>
void parseFile(osmpbf::OSMFileIn & inFile, TPBI_Processor processor);

///@processor (osmpbf::PrimitiveBlockInputAdaptor & pbi)
///@readBlobCount number of blobs to work on in parallel. If this is set to zero then this will default to max(omp_get_num_procs(), 1)
template<typename TPBI_Processor>
void parseFileOmp(osmpbf::OSMFileIn& inFile, TPBI_Processor processor, uint32_t readBlobCount = 0);

///@warning processor is passed by value! Pass a pointer to avoid copying
///@processor (osmpbf::PrimitiveBlockInputAdaptor & pbi) if the return value is not void, then the processing stops for ALL processors if its evaluated to false
///Every thread hold its own PrimitiveBlockInputAdaptor. You can set threadPrivateProcessor to true to always get the same pbi on a per-thread/@processor basis
///@threadCount if this is set to zero then this will default to max(omp_get_num_procs(), 1) or max(std::thread::hardware_concurrency(), 1)
///@readBlobCount number of blobs a single thread fetches to work upon before fetching new blobs
///@threadPrivateProcessor each thread will hold a copy of processor instead of sharing a single one
///@maxBlobsToRead maximum number of blobs to read
///@return number of blobs read
template<typename TPBI_Processor>
uint32_t parseFileCPPThreads(osmpbf::OSMFileIn& inFile, TPBI_Processor processor,
							uint32_t threadCount = 0,
							uint32_t readBlobCount = 1,
							bool threadPrivateProcessor = false,
							uint32_t maxBlobsToRead = 0xFFFFFFFF
						);


//definitions

template<typename TPBI_Processor>
void parseFile(OSMFileIn & inFile, TPBI_Processor processor)
{
	osmpbf::PrimitiveBlockInputAdaptor pbi;
	while (inFile.parseNextBlock(pbi))
	{
		if (pbi.isNull())
			continue;
		processor(pbi);
	}
}

template<typename TPBI_Processor>
void parseFileOmp(OSMFileIn & inFile, TPBI_Processor processor, uint32_t readBlobCount)
{
	if (!readBlobCount)
	{
		#if defined(_OPENMP)
		readBlobCount = std::max<int>(omp_get_num_procs(), 1);
		#else
		readBlobCount = 1;
		#endif
	}

	std::vector<osmpbf::BlobDataBuffer> pbiBuffers;
	bool processedFile = false;

	while (!processedFile)
	{
		pbiBuffers.clear();
		inFile.getNextBlocks(pbiBuffers, readBlobCount);
		uint32_t pbiCount = pbiBuffers.size();
		processedFile = (pbiCount < readBlobCount);

		#pragma omp parallel for schedule(dynamic)
		for(uint32_t i = 0; i < pbiCount; ++i)
		{
			osmpbf::PrimitiveBlockInputAdaptor pbi(pbiBuffers[i].data, pbiBuffers[i].availableBytes);
			pbiBuffers[i].clear();
			if (pbi.isNull())
			{
				continue;
			}
			processor(pbi);
		}
	}
}

template<typename TPBI_Processor>
uint32_t
parseFileCPPThreads(osmpbf::OSMFileIn& inFile, TPBI_Processor processor, uint32_t threadCount, uint32_t readBlobCount, bool threadPrivateProcessor, uint32_t maxBlobsToRead)
{
	typedef typename std::conditional<std::is_pointer<TPBI_Processor>::value, typename std::remove_pointer<TPBI_Processor>::type, TPBI_Processor>::type MyPbiProcessor;
// 	typedef typename std::remove_pointer<TPBI_Processor>::type MyPbiProcessor;
	typedef typename std::result_of<MyPbiProcessor(osmpbf::PrimitiveBlockInputAdaptor&)>::type PBIProcessorReturnType;
	typedef detail::ProcessorPtr<TPBI_Processor> ProcessorPtrCreator;
	if (!maxBlobsToRead)
		return 0;

	if (!threadCount)
	{
		threadCount = std::max<int>(std::thread::hardware_concurrency(), 1);
	}

	readBlobCount = std::max<uint32_t>(readBlobCount, 1);
	std::mutex mtx;
	uint32_t blobsRead = 0; //guarded by mtx
	std::atomic<bool> doProcessing(true);
	

	auto workFunc = [&inFile, &processor, &mtx, & blobsRead, &doProcessing, readBlobCount, threadPrivateProcessor, maxBlobsToRead]()
	{
		MyPbiProcessor * myP = ProcessorPtrCreator::ptr(processor);
		if (threadPrivateProcessor) {
			myP = new MyPbiProcessor(*myP);
		}
		osmpbf::PrimitiveBlockInputAdaptor pbi;
		std::vector<osmpbf::BlobDataBuffer> dbufs;

		while (doProcessing && blobsRead < maxBlobsToRead)
		{
			dbufs.clear();
			mtx.lock();
			bool haveNext = (blobsRead < maxBlobsToRead) && inFile.getNextBlocks(dbufs, std::min<uint32_t>(readBlobCount, maxBlobsToRead-blobsRead));
			blobsRead += dbufs.size();
			mtx.unlock();
			if (!haveNext) {//nothing left to do
				break;
			}
			for(osmpbf::BlobDataBuffer & dbuf : dbufs) {
				pbi.parseData(dbuf.data, dbuf.availableBytes);
				doProcessing = detail::PbiProcessor<MyPbiProcessor, PBIProcessorReturnType>::process(*myP, pbi) && doProcessing;
			}
		}
		if (threadPrivateProcessor) {
			delete myP;
		}
	};

	std::vector<std::thread> ts;
	ts.reserve(threadCount);
	for(uint32_t i(0); i < threadCount; ++i)
	{
		ts.push_back(std::thread(workFunc));
	}

	for(std::thread & t : ts)
	{
		t.join();
	}
	
	return blobsRead;
}


} // namespace osmpbf

#endif // OSMPBF_PARSEHELPERS_H
