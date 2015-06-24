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

#include <cstdint>

#include <iostream>
#include <set>

#include <osmpbf/blobfile.h>
#include <osmpbf/osmfile.h>

#include <osmpbf/primitiveblockinputadaptor.h>
#include <osmpbf/inode.h>
#include <osmpbf/iway.h>
#include <osmpbf/irelation.h>

#include <osmpbf/filter.h>

#include <osmpbf/primitiveblockoutputadaptor.h>
#include <osmpbf/onode.h>

template<class OSMInputPrimitive>
bool hasKeyId(OSMInputPrimitive & primitive, uint32_t id) {
	for (int i = 0; i < primitive.tagsSize(); i++)
		if (primitive.keyId(i) == id)
			return true;

	return false;
}

bool dataMatches(osmpbf::INode & nodeA, osmpbf::INode & nodeB) {
	if ((nodeA.id() != nodeB.id()) || (nodeA.lati() != nodeB.lati()) || (nodeA.loni() != nodeB.loni()) || (nodeA.tagsSize() != nodeB.tagsSize()))
		return false;

	for (int i = 0; i < nodeA.tagsSize(); ++i) {
		if (nodeB.valueByKey(nodeA.key(i)) != nodeA.value(i))
			return false;
	}

	return true;
}

int compare(char * inFileName, char * matchFileName, bool verbose) {
	if (!matchFileName) {
		std::cerr << "compare file paremeter is missing" << std::endl;
		return -1;
	}

	osmpbf::BlobFileIn inFileA(inFileName);
	osmpbf::BlobDataBuffer bufferA;
	inFileA.setVerboseOutput(verbose);

	osmpbf::BlobFileIn inFileB(matchFileName);
	osmpbf::BlobDataBuffer bufferB;
	inFileB.setVerboseOutput(verbose);

	inFileA.open();
	inFileB.open();

	bool match = true;
	do {
		inFileA.readBlob(bufferA);
		inFileB.readBlob(bufferB);

		if (bufferA.availableBytes == bufferB.availableBytes) {
			for (uint32_t i = 0; i < bufferA.availableBytes; i++)
				if (bufferA.data[i] != bufferB.data[i]) {
					std::cout << "found blob data missmatch" << std::endl;
					match = false;
					break;
				}
		}
	} while (match && bufferA.type && bufferB.type);

	if (match)
		std::cout << "the files match on blob level" << std::endl;

	inFileB.close();
	inFileB.close();

	return match ? 0 : -1;
}

int copyBlobs(char * inFileName, char * outFileName, bool verbose) {
	if (!outFileName) {
		std::cerr << "output file parameter is missing" << std::endl;
		return -1;
	}

	osmpbf::BlobFileIn inFile(inFileName);
	osmpbf::BlobFileOut outFile(outFileName);

	inFile.setVerboseOutput(verbose);
	outFile.setVerboseOutput(verbose);

	osmpbf::BlobDataBuffer buffer;

	inFile.open();
	outFile.open();

	bool writeOk = false;

	do {
		inFile.readBlob(buffer);
		writeOk = outFile.writeBlob(buffer);

	} while (buffer.type && writeOk);

	inFile.close();
	outFile.close();

	return 0;
}

int printStats(char * inputFileName, bool verbose) {
	osmpbf::BlobFileIn inFile(inputFileName);
	osmpbf::BlobDataBuffer buffer;

	inFile.setVerboseOutput(verbose);
	inFile.open();

	osmpbf::PrimitiveBlockInputAdaptor pbi;

	uint64_t waysCount = 0, nodesCount = 0, relationsCount = 0, denseCount = 0;

	int32_t wayRefsMin = INT32_MAX, wayRefsMax = 0;
	uint32_t wayRefsSum = 0;

	int32_t relationRefsMin = INT32_MAX, relationRefsMax = 0;
	uint32_t relationRefsSum = 0;

	do {
		inFile.readBlob(buffer);
		if (buffer.type != osmpbf::BLOB_OSMData)
			continue;

		pbi.parseData(buffer.data, buffer.availableBytes);

		if (!pbi.isNull()) {
			waysCount += pbi.waysSize();
			nodesCount += pbi.nodesSize();
			denseCount += pbi.nodesSize(osmpbf::DenseNode);
			relationsCount += pbi.relationsSize();

			for (osmpbf::IWayStream way = pbi.getWayStream(); !way.isNull(); way.next()) {
				wayRefsSum += way.refsSize();

				wayRefsMin = (wayRefsMin > way.refsSize()) ? way.refsSize() : wayRefsMin;
				wayRefsMax = (wayRefsMax < way.refsSize()) ? way.refsSize() : wayRefsMax;
			}

			for (osmpbf::IRelationStream relation = pbi.getRelationStream(); !relation.isNull(); relation.next()) {
				relationRefsSum += relation.membersSize();

				relationRefsMin = (relationRefsMin > relation.membersSize()) ? relation.membersSize() : relationRefsMin;
				relationRefsMax = (relationRefsMax < relation.membersSize()) ? relation.membersSize() : relationRefsMax;
			}
		}
	} while (buffer.type);

	inFile.close();

	std::cout << "\nstatistics over all blocks (total, ref min, ref max, ref avg, ref sum):" <<
		"\n- nodes:     " << nodesCount;

	if (denseCount) std::cout<< "(dense: " << denseCount <<  ')';

	std::cout << "\n- ways:      " << waysCount;
	if (waysCount) std::cout << ", " << wayRefsMin << ", " << wayRefsMax << ", " << (waysCount ? wayRefsSum / waysCount : 0) << ", " << wayRefsSum;

	std::cout << "\n- relations: " << relationsCount;
	if (relationsCount)
		std::cout << ", " << relationRefsMin << ", " << relationRefsMax << ", " << (relationsCount ? relationRefsSum / relationsCount : 0) << ", " << relationRefsSum;

	std::cout << std::endl;

	return 0;
}

void dump(osmpbf::INode & node) {
	std::cout << "[Node]" <<
		"\nid = " << node.id() <<
		"\nlat = " << node.lati() <<
		"\nlon = " << node.loni() <<
		"\ntags ([#] <key> = <value>):" << std::endl;
	if (node.tagsSize())
		for (int i = 0; i < node.tagsSize(); i++)
			std::cout << '[' << i << "] " << node.key(i) << " = " << node.value(i) << std::endl;
	else
		std::cout << " <none>" << std::endl;
}

void dump(osmpbf::IWay & way) {
	std::cout << "[Way]" <<
		"\nid = " << way.id() <<
		"\nrefs_size = " << way.refsSize() <<
		"\nrefs:" << std::endl;
	if (way.refsSize()) {
		generics::DeltaFieldConstForwardIterator<int64_t> it;

		for(it = way.refBegin(); it != way.refEnd(); ++it) {
			std::cout << '[' << *it << ']';
		}
		std::cout << std::endl;
	}
	else
		std::cout << " <none>" << std::endl;

	std::cout << "keys, vals:" << std::endl;
	if (way.tagsSize())
		for (int i = 0; i < way.tagsSize(); i++)
			std::cout << '[' << i << "] " << way.key(i) << " = " << way.value(i) << std::endl;
	else
		std::cout << " <none>" << std::endl;
}

std::ostream & operator<<(std::ostream & stream, osmpbf::PrimitiveType pt) {
	switch (pt) {
	case osmpbf::NodePrimitive:
		return stream << "node";
	case osmpbf::WayPrimitive:
		return stream << "way";
	case osmpbf::RelationPrimitive:
		return stream << "relation";
	default:
		return stream;
	}
}

void dump(osmpbf::IRelation & relation) {
	std::cout << "[Relation]" <<
		"\nid = " << relation.id() <<
		"\nmembers_size = " << relation.membersSize() <<
		"\nmembers:" << std::endl;
	if (relation.membersSize()) {
		for (osmpbf::IMemberStream memberStream = relation.getMemberStream(); !memberStream.isNull(); memberStream.next()) {
			std::cout << '[' << memberStream.id() <<
				"] type = " << memberStream.type() <<
				"; role = " << memberStream.role() << std::endl;
		}
	}
	else
		std::cout << " <none>" << std::endl;

	std::cout << "keys, vals:" << std::endl;
	if (relation.tagsSize())
		for (int i = 0; i < relation.tagsSize(); i++)
			std::cout << '[' << i << "] " << relation.key(i) << " = " << relation.value(i) << std::endl;
	else
		std::cout << " <none>" << std::endl;
}

int dumpAll(char * inputFileName, bool verbose) {
	osmpbf::OSMFileIn inFile(inputFileName, verbose);
	osmpbf::PrimitiveBlockInputAdaptor pbi;

	if (!inFile.open())
		return -1;

	while (inFile.parseNextBlock(pbi)) {
		if (pbi.isNull())
			continue;

		if (pbi.waysSize()) {
			std::cout << "found " << pbi.waysSize() << " ways:" << std::endl;
			for (osmpbf::IWayStream way = pbi.getWayStream(); !way.isNull(); way.next())
				dump(way);
		}

		if (pbi.nodesSize()) {
			std::cout << "found " << pbi.nodesSize() << " nodes:" << std::endl;
			for (osmpbf::INodeStream node = pbi.getNodeStream(); !node.isNull(); node.next())
				dump(node);
		}

		if (pbi.relationsSize()) {
			std::cout << "found " << pbi.relationsSize() << " relations:" << std::endl;
			for (osmpbf::IRelationStream relationStream = pbi.getRelationStream(); !relationStream.isNull(); relationStream.next())
				dump(relationStream);
		}
	}

	inFile.close();

	return 0;
}

int dumpMatch(char * inputFileName, char * matchString, bool verbose) {
	if (!matchString)
		return dumpAll(inputFileName, verbose);

	osmpbf::OSMFileIn inFile(inputFileName, verbose);

	if (!inFile.open())
		return -1;

	uint64_t matchingWaysCount = 0;
	uint64_t matchingNodesCount = 0;
	uint64_t matchingRelationsCount = 0;
	uint64_t waysCount = 0;
	uint64_t nodesCount = 0;
	uint64_t relationsCount = 0;

	std::string keyString = matchString;

	std::size_t delimiter = keyString.find('=');
	std::string valueString;

	if (delimiter != std::string::npos) {
		if (delimiter + 1 < keyString.size())
			valueString = keyString.substr(delimiter + 1);

		keyString.erase(delimiter);
	}

	std::cout << "looking for: key = \"" << keyString << "\" value = \"" << valueString << '\"' << std::endl;

	osmpbf::AbstractTagFilter * filter = valueString.size() ?
		new osmpbf::KeyValueTagFilter(keyString, valueString) :
		new osmpbf::KeyOnlyTagFilter(keyString);

	keyString.clear();
	valueString.clear();

	osmpbf::PrimitiveBlockInputAdaptor pbi;
	filter->assignInputAdaptor(&pbi);

	while (inFile.parseNextBlock(pbi)) {
		if (pbi.isNull())
			continue;

		waysCount += pbi.waysSize();
		nodesCount += pbi.nodesSize();
		relationsCount += pbi.relationsSize();

		if (!filter->rebuildCache())
			continue;

		uint32_t matchCount;

		if (pbi.waysSize()) {
			matchCount = 0;

			for (osmpbf::IWayStream wayStream = pbi.getWayStream(); !wayStream.isNull(); wayStream.next()) {
				if (filter->matches(wayStream)) {
					matchCount++;
					dump(wayStream);
				}
			}

			std::cout << "found " << matchCount << " matches in " << pbi.waysSize() << " ways" << std::endl;
			matchingWaysCount += matchCount;
		}

		if (pbi.nodesSize()) {
			matchCount = 0;

			for (osmpbf::INodeStream nodeStream = pbi.getNodeStream(); !nodeStream.isNull(); nodeStream.next()) {
				if (filter->matches(nodeStream)) {
					matchCount++;
					dump(nodeStream);
				}
			}

			std::cout << "found " << matchCount << " matches in " << pbi.nodesSize() << " nodes" << std::endl;
			matchingNodesCount += matchCount;
		}

		if (pbi.relationsSize()) {
			matchCount = 0;

			for (osmpbf::IRelationStream relationStream = pbi.getRelationStream(); !relationStream.isNull(); relationStream.next()) {
				if (filter->matches(relationStream)) {
					matchCount++;
					dump(relationStream);
				}
			}

			std::cout << "found " << matchCount << " matches in " << pbi.relationsSize() << " relations" << std::endl;
			matchingRelationsCount += matchCount;
		}
	}

	delete filter;

	std::cout << "\nstatistics over all blocks (matches, total):" <<
		"\n- nodes:     " << matchingNodesCount << ", " << nodesCount <<
		"\n- ways:      " << matchingWaysCount << ", " << waysCount <<
		"\n- relations: " << matchingRelationsCount << ", " << relationsCount << std::endl;

	inFile.close();
	return 0;
}

int extract(const char * inputFileName, const char * outputFileName, const char * matchString, bool verbose) {
	if (!matchString) {
		std::cerr << "ERROR: no match string supplied" << std::endl;
		return -1;
	}

	if (!outputFileName) {
		std::cerr << "ERROR: output file parameter is missing" << std::endl;
		return -1;
	}

	osmpbf::BlobFileIn inFile(inputFileName);
	osmpbf::BlobFileOut outFile(outputFileName);

	osmpbf::BlobDataBuffer buffer;

	inFile.setVerboseOutput(verbose);
	outFile.setVerboseOutput(verbose);

	inFile.open();
	outFile.open();

	osmpbf::PrimitiveBlockOutputAdaptor pbo;
	do {
		inFile.readBlob(buffer);

		if (buffer.type == osmpbf::BLOB_OSMData) {
			osmpbf::PrimitiveBlockInputAdaptor pbi(buffer.data, buffer.availableBytes);

			int keyStringIndex;

			for (keyStringIndex = 0; keyStringIndex < pbi.stringTableSize(); keyStringIndex++) {
				if (pbi.queryStringTable(keyStringIndex) == matchString) break;
			}

			if (keyStringIndex == pbi.stringTableSize())
				continue;

			for (osmpbf::IWayStream wayStream = pbi.getWayStream(); !wayStream.isNull(); wayStream.next())
				if (hasKeyId<osmpbf::IWayStream>(wayStream, keyStringIndex))
					pbo << wayStream;

			for (osmpbf::INodeStream nodeStream = pbi.getNodeStream(); !nodeStream.isNull(); nodeStream.next())
				if (hasKeyId<osmpbf::INodeStream>(nodeStream, keyStringIndex))
					pbo << nodeStream;

			if (pbo.waysSize() || pbo.nodesSize(osmpbf::PlainNode) || pbo.nodesSize(osmpbf::DenseNode)) {
				std::string outputBuffer;

				pbo.setGranularity(pbi.granularity());
				pbo.setLatOffset(pbi.latOffset());
				pbo.setLonOffset(pbi.lonOffset());

				pbo.flush(outputBuffer);
				outFile.writeBlob(osmpbf::BLOB_OSMData, outputBuffer.data(), outputBuffer.size(), true);
			}
		}
	} while(buffer.type);

	inFile.close();
	outFile.close();

	return 0;
}

int extractWays(const char * inputFileName, const char * outputFileName, bool verbose) {
	if (!outputFileName) {
		std::cerr << "ERROR: output file parameter is missing" << std::endl;
		return -1;
	}

	osmpbf::BlobFileIn inFile(inputFileName);
	osmpbf::BlobFileOut outFile(outputFileName);

	osmpbf::BlobDataBuffer buffer;

	inFile.setVerboseOutput(verbose);
	outFile.setVerboseOutput(verbose);

	osmpbf::PrimitiveBlockOutputAdaptor pbo;

	std::set< int64_t > nodes;

	inFile.open();
	outFile.open();

	uint32_t pbiCount = 0;

	// collect ways and node ids
	std::cout << "collecting ways and node refs ..."; std::cout.flush();
	do {
		inFile.readBlob(buffer);

		if (buffer.type == osmpbf::BLOB_OSMData) {
			osmpbf::PrimitiveBlockInputAdaptor pbi(buffer.data, buffer.availableBytes);

			// extract node ids and
			for (osmpbf::IWayStream way = pbi.getWayStream(); !way.isNull(); way.next()) {
				pbo << way;

				generics::DeltaFieldConstForwardIterator<int64_t> it;

				for(it = way.refBegin(); it != way.refEnd(); ++it)
					nodes.insert(*it);

				// flush ways
				if (pbo.waysSize() == 8000) {
					std::string outputBuffer;
					pbo.flush(outputBuffer);
					outFile.writeBlob(osmpbf::BLOB_OSMData, outputBuffer.data(), outputBuffer.size(), true);
				}
			}

			std::cout << '\r';
			std::cout << "collecting ways and node refs ... " << ++pbiCount;
			std::cout.flush();
		}
	} while(buffer.type);
	std::cout << std::endl;

	pbiCount = 0;
	inFile.seek(0);

	// collect node data
	std::cout << "collecting node data ..."; std::cout.flush();
	do {
		inFile.readBlob(buffer);

		if (buffer.type == osmpbf::BLOB_OSMData) {
			osmpbf::PrimitiveBlockInputAdaptor pbi(buffer.data, buffer.availableBytes);

			// extract node ids and
			for (osmpbf::INodeStream node = pbi.getNodeStream(); !node.isNull(); node.next()) {
				if (nodes.count(node.id()))
					pbo << node;

				// flush nodes
				if (pbo.nodesSize(osmpbf::PlainNode) + pbo.nodesSize(osmpbf::DenseNode) == 8000) {
					std::string outputBuffer;
					pbo.flush(outputBuffer);
					outFile.writeBlob(osmpbf::BLOB_OSMData, outputBuffer.data(), outputBuffer.size(), true);
				}
			}

			std::cout << '\r';
			std::cout << "collecting node data ... " << ++pbiCount;
			std::cout.flush();
		}
	} while(buffer.type);
	std::cout << std::endl;

	inFile.close();
	outFile.close();

	return 0;
}

/* parameters:
 * -o file_name ... out file
 * -c file_name ... file to compare with
 * -m match_string ... work only on primitives matching
 * -v ... verbose output
 */
struct MyParameters {
	char * inputFileName;
	char * outputFileName;
	char * compareFileName;
	char * matchString;
	bool verbose;

	MyParameters(int argc, char * argv[]) :
		inputFileName(NULL),
		outputFileName(NULL),
		compareFileName(NULL),
		matchString(NULL),
		verbose(false)
	{
		int p = 2;
		while (p < argc - 1) {
			if (argv[p][0] == '-') {
				switch (argv[p][1]) {
				case 'o':
					p++;
					if ((p >= argc - 1) || (argv[p][0] == '-')) {
						std::cerr << "ERROR: invalid output file parameter" << std::endl;
						return;
					}

					outputFileName = argv[p];
					break;
				case 'c':
					p++;
					if ((p >= argc - 1) || (argv[p][0] == '-')) {
						std::cerr << "ERROR: invalid compare file parameter" << std::endl;
						return;
					}

					compareFileName = argv[p];
					break;
				case 'm':
					p++;
					if ((p >= argc - 1) || (argv[p][0] == '-')) {
						std::cerr << "ERROR: invalid match parameter" << std::endl;
						return;
					}

					matchString = argv[p];
					break;
				case 'v':
					verbose = true;
					break;
				default:
					std::cerr << "WARNING: unknown parameter \"" << argv[p] << '\"' << std::endl;
					break;
				}
			}

			p++;
		}

		inputFileName = argv[p];
	};
};

#define MODE_COMPARE 'c'
#define MODE_COPY_BLOBS 'b'
#define MODE_EXTRACT 'e'
#define MODE_DUMP_MATCH 'd'
#define MODE_DUMP_ALL 'D'
#define MODE_BLOB_STATS 's'
#define MODE_BLOB_DATA_STATS 'S'
#define MODE_EXTRACT_WAYS 'w'

int main(int argc, char * argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <mode> [parameters] " << "file" << std::endl;
		return -1;
	}

	MyParameters params(argc, argv);

	if (!params.inputFileName) {
		std::cerr << "ERROR: input file parameter is missing" << std::endl;
		return -1;
	}

	switch (argv[1][0]) {
	case MODE_COPY_BLOBS:
		return copyBlobs(params.inputFileName, params.outputFileName, params.verbose);
	case MODE_COMPARE:
		return compare(params.inputFileName, params.compareFileName, params.verbose);
	case MODE_BLOB_STATS:
		return printStats(params.inputFileName, params.verbose);
	case MODE_DUMP_ALL:
		return dumpAll(params.inputFileName, params.verbose);
	case MODE_DUMP_MATCH:
		return dumpMatch(params.inputFileName, params.matchString, params.verbose);
	case MODE_EXTRACT:
		return extract(params.inputFileName, params.outputFileName, params.matchString, params.verbose);
	case MODE_EXTRACT_WAYS:
		return extractWays(params.inputFileName, params.outputFileName, params.verbose);
	default:
		std::cerr << "ERROR: unknown mode \"" << argv[1][0] << '\"' << std::endl;
		return -1;
	}
}
