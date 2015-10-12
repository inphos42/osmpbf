/*
    This file is part of the osmpbf library.

    Copyright(c) 2014 Daniel Bahrdt.

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
#include <iostream>
#if defined(__OPENMP)
#include <omp.h>
#endif
#include <osmpbf/osmfile.h>
#include <osmpbf/inode.h>
#include <osmpbf/iway.h>
#include <osmpbf/irelation.h>
#include <osmpbf/filter.h>
#include <osmpbf/primitiveblockinputadaptor.h>


inline std::string primitiveTypeToString(osmpbf::PrimitiveType t) {
	switch (t) {
	case osmpbf::PrimitiveType::NodePrimitive:
		return "node";
	case osmpbf::PrimitiveType::WayPrimitive:
		return "way";
	case osmpbf::PrimitiveType::RelationPrimitive:
		return "relation";
	default:
		return "invalid";
	};
}

void parseBlock(osmpbf::PrimitiveBlockInputAdaptor & pbi) {
	//using filters is straight forward
	osmpbf::AndTagFilter andFilter({
									new osmpbf::KeyOnlyTagFilter("name"),
									new osmpbf::KeyMultiValueTagFilter("highway", {"path", "residential"})
									});


	//build the id cache for faster queries (this is not neccessary)
	if (!andFilter.rebuildCache()) {
// 		std::cout << "No matching elements in this block" << std::endl;
	}
	
	if (pbi.nodesSize()) {
		for (osmpbf::INodeStream node = pbi.getNodeStream(); !node.isNull(); node.next()) {
			std::cout << "<node id=" << node.id() << " lat=" << node.latd() << " lon=" << node.lond()  << ">"<< std::endl;
			for(uint32_t i = 0, s = node.tagsSize();  i < s; ++i) {
				std::cout << "\t<tag k=" << node.key(i) << " v=" << node.value(i) << ">" << std::endl;
			}
			std::cout << "</node>" << std::endl;
			//to check if a primitive matches the filter use
			if (andFilter.matches(node)) {
				;//do something
			}
		}
	}

	if (pbi.waysSize()) {
		for (osmpbf::IWayStream way = pbi.getWayStream(); !way.isNull(); way.next()) {
			std::cout << "<way id=" << way.id() << ">" << std::endl;
			for(osmpbf::RefIterator refIt(way.refBegin()), refEnd(way.refEnd()); refIt != refEnd; ++refIt) {
				std::cout << "\t<nd ref=" << *refIt << "/>" << std::endl;
			}
			for(uint32_t i = 0, s = way.tagsSize();  i < s; ++i) {
				std::cout << "\t<tag k=" << way.key(i) << " v=" << way.value(i) << ">" << std::endl;
			}
			std::cout << "</way>" << std::endl;
		}
	}
	
	if (pbi.relationsSize()) {
		for(osmpbf::IRelationStream relation = pbi.getRelationStream(); !relation.isNull(); relation.next()) {
			std::cout << "<relation id=" << relation.id() << ">" << std::endl;
			for(osmpbf::IMemberStream mem(relation.getMemberStream()); !mem.isNull(); mem.next()) {
				std::cout << "\t<member type=" << primitiveTypeToString( mem.type() ) << " ref=" << mem.id() << " role=" << mem.role() << "/>" <<  std::endl;
			}
			for(uint32_t i = 0, s = relation.tagsSize();  i < s; ++i) {
				std::cout << "\t<tag k=" << relation.key(i) << " v=" << relation.value(i) << ">" << std::endl;
			}
			std::cout << "</relation>" << std::endl;
		}
	}
}

int main(int argc, char ** argv) {
	if (argc < 2) {
		std::cout << "Need in file" << std::endl;
	}
	
	std::string inputFileName(argv[1]);

	osmpbf::OSMFileIn inFile(inputFileName, false);

	if (!inFile.open()) {
		std::cout << "Failed to open " <<  inputFileName << std::endl;
		return -1;
	}

/*
	With some additionaly effort it's possible to parse the pbf in parallel which of course uses more memory

*/
#if defined(__OPENMP)
	uint32_t readBlobCount = omp_get_num_procs();
	bool processedFile = false;
	while (!processedFile) {
		std::vector<osmpbf::BlobDataBuffer> pbiBuffers = inFile.getNextBlocks(readBlobCount);
		uint32_t pbiCount = pbiBuffers.size();
		processedFile = (pbiCount < readBlobCount);
		#pragma omp parallel for
		for(uint32_t i = 0; i < pbiCount; ++i) {
			osmpbf::PrimitiveBlockInputAdaptor pbi(pbiBuffers[i].data, pbiBuffers[i].availableBytes);
			pbiBuffers[i].clear();
			if (pbi.isNull()) {
				continue;
			}
			parseBlock(pbi);
		}
	}
#else
	osmpbf::PrimitiveBlockInputAdaptor pbi;
	while (inFile.parseNextBlock(pbi)) {
		if (pbi.isNull())
			continue;
		parseBlock(pbi);
	}
#endif

	return 0;
}
