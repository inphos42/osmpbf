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

#include <iostream>
#include <deque>

#include <osmpbf/osmfilein.h>
#include <osmpbf/blobfile.h>
#include <osmpbf/primitiveblockinputadaptor.h>

#include "osmformat.pb.h"

namespace osmpbf {

// OSMFileIn

	OSMFileIn::OSMFileIn(const std::string & fileName, bool verboseOutput) :
		m_FileIn(new BlobFileIn(fileName)),
		m_FileHeader(NULL),
		m_DataOffset(0)
	{
		m_FileIn->setVerboseOutput(verboseOutput);
	}

	OSMFileIn::OSMFileIn(BlobFileIn * fileIn) :
		m_FileIn(fileIn),
		m_FileHeader(NULL),
		m_DataOffset(0)
	{}

	OSMFileIn::~OSMFileIn() {
		delete m_FileIn;
		delete m_FileHeader;
	}


	bool OSMFileIn::open() {
		if (m_FileIn->open() && parseHeader())
			return true;

		close();
		return false;
	}

	void OSMFileIn::close() {
		m_FileIn->close();
		m_DataBuffer.clear();
	}

	void OSMFileIn::dataSeek(osmpbf::OffsetType position) {
		m_FileIn->seek(m_DataOffset + position);
	}

	OffsetType OSMFileIn::dataPosition() const {
		return m_FileIn->position() - m_DataOffset;
	}

	OffsetType OSMFileIn::dataSize() const {
		return m_FileIn->size() - m_DataOffset;
	}

	OffsetType OSMFileIn::totalSize() const {
		return m_FileIn->size();
	}

	bool OSMFileIn::getNextBlock(BlobDataBuffer & buffer) {
		m_FileIn->readBlob(buffer);
		return buffer.type != BLOB_Invalid;
	}

	bool OSMFileIn::getNextBlocks(BlobDataMultiBuffer & buffers, uint32_t num) {
		// ensure there is enough space
		buffers.reserve(num);

		// read (all) buffers
		uint32_t i = 0;
		while ((i < num || num < 0) && getNextBlock(buffers[i]))
			++i;

		// resize buffers and check if all parsing
		buffers.resize(i);
		return (i = num) || (num < 0);
	}

	bool OSMFileIn::parseNextBlock(PrimitiveBlockInputAdaptor & adaptor) {
		m_FileIn->readBlob(m_DataBuffer);
		adaptor.parseData(m_DataBuffer.data, m_DataBuffer.availableBytes);
		return m_DataBuffer.type != BLOB_Invalid;
	}

	bool OSMFileIn::skipBlock() {
		return m_FileIn->skipBlob();
	}

	bool OSMFileIn::readBlock() {
		m_FileIn->readBlob(m_DataBuffer);
		return m_DataBuffer.type != BLOB_Invalid;
	}

	bool OSMFileIn::parseHeader() {
		m_FileIn->readBlob(m_DataBuffer);

		if (m_DataBuffer.type != BLOB_OSMHeader) {
			std::cerr << "ERROR: OSM header block not found" << std::endl;
			return false;
		}

		m_FileHeader = new crosby::binary::HeaderBlock;

		// parse and check header
		if (!m_FileHeader->ParseFromArray(m_DataBuffer.data, m_DataBuffer.availableBytes)) {
			std::cerr << "ERROR: invalid OSM header" << std::endl;
			delete m_FileHeader;
			m_FileHeader = NULL;
			return false;
		}

		m_DataOffset = m_FileIn->position();

		// check requirements
		bool noneMissing = true;
		m_MissingFeatures.resize(requiredFeaturesSize(), false);
		for (int i = 0; i < requiredFeaturesSize(); ++i) {
			if ((m_FileHeader->required_features(i) != "OsmSchema-V0.6") &&
				(m_FileHeader->required_features(i) != "DenseNodes"))
			{
				std::cerr << "ERROR: missing required feature of input data: \"" << m_FileHeader->required_features(i) << '\"' << std::endl;
				m_MissingFeatures[i] = true;
				noneMissing = false;
			}
		}

		if (noneMissing)
			m_MissingFeatures.clear();

		return m_MissingFeatures.empty();
	}

	int OSMFileIn::requiredFeaturesSize() const {
		return m_FileHeader->required_features_size();
	}

	int OSMFileIn::optionalFeaturesSize() const {
		return m_FileHeader->optional_features_size();
	}

	const std::string & OSMFileIn::requiredFeatures(int index) const {
		return m_FileHeader->required_features(index);
	}

	const std::string & OSMFileIn::optionalFeatures(int index) const {
		return m_FileHeader->optional_features(index);
	}

	int64_t OSMFileIn::minLat() const {
		return m_FileHeader->has_bbox() ? m_FileHeader->bbox().bottom() : 0;
	}

	int64_t OSMFileIn::maxLat() const {
		return m_FileHeader->has_bbox() ? m_FileHeader->bbox().top() : 0;
	}

	int64_t OSMFileIn::minLon() const {
		return m_FileHeader->has_bbox() ? m_FileHeader->bbox().left() : 0;
	}

	int64_t OSMFileIn::maxLon() const {
		return m_FileHeader->has_bbox() ? m_FileHeader->bbox().right() : 0;
	}
}
