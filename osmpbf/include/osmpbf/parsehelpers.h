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

#include <omp.h>
#include <mutex>
#include <thread>

namespace osmpbf
{

///@processor (osmpbf::PrimitiveBlockInputAdaptor & pbi)
template<typename TPBI_Processor>
void parseFile(osmpbf::OSMFileIn & inFile, TPBI_Processor processor);

///@processor (osmpbf::PrimitiveBlockInputAdaptor & pbi)
///@readBlobCount number of blobs to work on in parallel. If this is set to zero then this will default to max(omp_get_num_procs(), 1)
template<typename TPBI_Processor>
void parseFileOmp(osmpbf::OSMFileIn& inFile, TPBI_Processor processor, uint32_t readBlobCount = 0);

///@processor (osmpbf::PrimitiveBlockInputAdaptor & pbi)
///@threadCount if this is set to zero then this will default to max(omp_get_num_procs(), 1) or max(std::thread::hardware_concurrency(), 1)
///@readBlobCount number of blobs a single thread fetches to work upon before fetching new blobs
///@threadPrivateProcessor each thread will hold a copy of processor instead of sharing a single one
template<typename TPBI_Processor>
void parseFileCPPThreads(osmpbf::OSMFileIn& inFile, TPBI_Processor processor, uint32_t threadCount = 0, uint32_t readBlobCount = 1, bool threadPrivateProcessor = false);


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
		readBlobCount = std::max<int>(omp_get_num_procs(), 1);
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
void parseFileCPPThreads(osmpbf::OSMFileIn& inFile, TPBI_Processor processor, uint32_t threadCount, uint32_t readBlobCount, bool threadPrivateProcessor)
{
	if (!threadCount)
	{
		threadCount = std::max<int>(std::thread::hardware_concurrency(), 1);
	}

	readBlobCount = std::max<uint32_t>(readBlobCount, 1);
	std::mutex mtx;

	auto workFunc = [&inFile, &processor, &mtx, readBlobCount, threadPrivateProcessor]()
	{
		TPBI_Processor * myP = (threadPrivateProcessor? new TPBI_Processor(processor) : &processor);
		osmpbf::PrimitiveBlockInputAdaptor pbi;
		std::vector<osmpbf::BlobDataBuffer> dbufs;

		while (true)
		{
			dbufs.clear();
			mtx.lock();
			bool haveNext = inFile.getNextBlocks(dbufs, readBlobCount);
			mtx.unlock();
			if (!haveNext) {//nothing left to do
				break;
			}
			for(osmpbf::BlobDataBuffer & dbuf : dbufs) {
				pbi.parseData(dbuf.data, dbuf.availableBytes);
				(*myP)(pbi);
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
}


} // namespace osmpbf

#endif // OSMPBF_PARSEHELPERS_H
