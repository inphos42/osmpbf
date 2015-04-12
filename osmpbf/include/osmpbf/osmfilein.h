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

#ifndef OSMPBF_OSMFILEIN_H
#define OSMPBF_OSMFILEIN_H

#include <osmpbf/blobdata.h>
#include <osmpbf/typelimits.h>
#include <osmpbf/pbf_prototypes.h>

#include <string>
#include <vector>

namespace osmpbf
{

class PrimitiveBlockInputAdaptor;
class BlobFileIn;

typedef std::vector<BlobDataBuffer> BlobDataMultiBuffer;

class OSMFileIn
{
public:
	OSMFileIn(const std::string & fileName, bool verboseOutput = false);
	OSMFileIn(BlobFileIn * fileIn);
	~OSMFileIn();

	bool open();
	void close();
	void reset();
	void dataSeek(OffsetType position);
	OffsetType dataPosition() const;
	OffsetType dataSize() const;

	OffsetType totalSize() const;


	bool hasNext() const;

	bool readBlock();
	bool skipBlock();

	/**
	 * copy next block into data buffer
	 * not thread safe
	 *
	 * @param buffer target buffer
	 */
	bool getNextBlock(BlobDataBuffer & buffer);

	/**
	 * copy next blocks into data buffers
	 * not thread-safe
	 *
	 * @param buffers target container of buffers
	 * @param num number of blocks to copy, set to -1 to copy all remaining blocks
	 */
	bool getNextBlocks(BlobDataMultiBuffer & buffers, int num);

	///@param adaptor parse next block by @adaptor, not thread-safe
	bool parseNextBlock(PrimitiveBlockInputAdaptor & adaptor);

	inline const BlobDataBuffer & blockBuffer() const { return m_DataBuffer; }
	inline void clearBlockBuffer() { m_DataBuffer.clear(); }

	inline bool parserMeetsRequirements() const { return m_MissingFeatures.empty(); }

	int requiredFeaturesSize() const;
	int optionalFeaturesSize() const;

	const std::string & requiredFeatures(int index) const;
	const std::string & optionalFeatures(int index) const;
	inline bool requiredFeatureMissing(int index) const { return m_MissingFeatures.empty() ? false : m_MissingFeatures.at(index); }

	// bounding box in nanodegrees
	int64_t minLat() const;
	int64_t maxLat() const;

	int64_t minLon() const;
	int64_t maxLon() const;

protected:
	OSMFileIn() = delete;
	OSMFileIn(const OSMFileIn & other) = delete;
	OSMFileIn & operator=(const OSMFileIn & other) = delete;

	BlobFileIn * m_FileIn;
	BlobDataBuffer m_DataBuffer;

	crosby::binary::HeaderBlock * m_FileHeader;
	std::vector< bool > m_MissingFeatures;

	OffsetType m_DataOffset;

	bool parseHeader();
};

} // namespace osmpbf

#endif // OSMPBF_OSMFILEIN_H
