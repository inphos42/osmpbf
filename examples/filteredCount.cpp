/*
    This file is part of the osmpbf library.

    Copyright(c) 2015 Daniel Bahrdt.

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
#include <osmpbf/parsehelpers.h>
#include <osmpbf/inode.h>
#include <osmpbf/iway.h>
#include <osmpbf/irelation.h>
#include <osmpbf/filter.h>

/**
  * This is a small example to demonstrate the use of filters together with threads.
  * Filters are NOT! thread-safe. We circumvent this by using only thread-local filters.
  */

struct SharedState {
	std::mutex lock;
	uint64_t nodeCount;
	uint64_t wayCount;
	uint64_t relationCount;
	SharedState() : nodeCount(0), wayCount(0), relationCount(0) {}
};

struct MyCounter {
	SharedState * state;
	osmpbf::CopyFilterPtr filter; //automatically create a deep copy of filter dag
	uint64_t nodeCount;
	uint64_t wayCount;
	uint64_t relationCount;
	MyCounter(SharedState * state, const osmpbf::RCFilterPtr & filter) : state(state), filter(filter), nodeCount(0), wayCount(0), relationCount(0) {}
	MyCounter(const MyCounter & other) : state(other.state), filter(other.filter), nodeCount(0), wayCount(0), relationCount(0) {}
	void operator()(osmpbf::PrimitiveBlockInputAdaptor & pbi) {
		filter->assignInputAdaptor(&pbi);
		//we can rebuild the cache ourselfs for early termination
// 		if (!filter->rebuildCache()) {
// 			return;
// 		}
		nodeCount = wayCount = relationCount = 0;
		for(osmpbf::INodeStream node(pbi.getNodeStream()); !node.isNull(); node.next()) {
			if (filter->matches(node)) {
				++nodeCount;
			}
		}
		for(osmpbf::IWayStream way(pbi.getWayStream()); !way.isNull(); way.next()) {
			if (filter->matches(way)) {
				++wayCount;
			}
		}
		for(osmpbf::IRelationStream rel(pbi.getRelationStream()); !rel.isNull(); rel.next()) {
			if (filter->matches(rel)) {
				++relationCount;
			}
		}
		//now flush everything to shared state
		std::lock_guard<std::mutex> lck(state->lock);
		state->nodeCount += nodeCount;
		state->wayCount += wayCount;
		state->relationCount += relationCount;
	}
};

void help() {
	std::cout << "Count the number of primitives in a osm.pbf file matching specified tags\n";
	std::cout << "prg [-k <key> [-k]] [-kv <key> <value> [-kv]] [-t number_of_threads] [-b number_of_blocks_per_fetch] filename\n";
	std::cout << std::flush;
}


int main(int argc, char ** argv) {
	std::vector<std::string> keys;
	std::vector< std::pair<std::string, std::string> > kvs;
	osmpbf::RCFilterPtr filter;
	std::string fileName;
	SharedState state;
	uint32_t threadCount = 2; //use 2 threads, usually 4 are more than enough
	uint32_t readBlobCount = 2; //parse 2 blocks at once

	
	for(int i(0); i < argc; ++i) {
		std::string token(argv[i]);
		if (token == "-k" && i+1 < argc) {
			keys.emplace_back(argv[i+1]);
			++i;
		}
		else if (token == "-kv" && i+2 < argc) {
			kvs.emplace_back(std::string(argv[i+1]), std::string(argv[i+2]));
			i+=2;
		}
		else if (token == "-t" && i+1 < argc) {
			threadCount = ::atoi(argv[i+1]);
			++i;
		}
		else if (token == "-b" && i+1 < argc) {
			readBlobCount = ::atoi(argv[i+1]);
			++i;
		}
		else if(token == "--help" || token == "-h") {
			help();
			return 0;
		}
		else {
			fileName = token;
		}
	}
	
	
	osmpbf::OSMFileIn inFile(fileName);
	if (!inFile.open()) {
		std::cerr << "Could not open file " << fileName << std::endl;
		return -1;
	}
	
	{//setup filters
		auto orFilter = new osmpbf::OrTagFilter({new osmpbf::MultiKeyTagFilter(keys.begin(), keys.end())});
		for(auto x : kvs) {
			orFilter->addChild(new osmpbf::KeyValueTagFilter(x.first, x.second));
		}
		filter.reset(orFilter); //takes ownership of orFilter
	}
	
	bool threadPrivateProcessor = true; //set to true so that MyCounter is copied
	
	osmpbf::parseFileCPPThreads(inFile, MyCounter(&state, filter), threadCount, readBlobCount, threadPrivateProcessor);
	
	std::cout << "File " << fileName << " has the following amounts of matching primitives:\n";
	std::cout << "Nodes: " << state.nodeCount << "\n";
	std::cout << "Ways: " << state.wayCount << "\n";
	std::cout << "Relations: " << state.relationCount<< "\n";
	std::cout << std::flush;
	return 0;
}