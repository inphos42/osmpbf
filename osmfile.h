/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2013 Oliver Groß.

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

#ifndef OSMPBF_OSMFILE_H
#define OSMPBF_OSMFILE_H

#include <string>
#include <vector>

#include "blobdata.h"

#include "typelimits.h"

namespace crosby {
	namespace binary {
		class HeaderBlock;
	}
}

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;
	class BlobFileIn;

	class OSMFileIn {
	public:
		OSMFileIn(const std::string & fileName, bool verboseOutput = false);
		OSMFileIn(BlobFileIn * fileIn);
		~OSMFileIn();

		bool open();
		void close();

		void dataSeek(OffsetType position);
		OffsetType dataPosition() const;
		OffsetType dataSize() const;

		OffsetType totalSize() const;

		bool readBlock();
		bool skipBlock();

		///@param databuffer copy next block into data buffer, not thread safe
		bool getNextBlock(BlobDataBuffer & databuffer);
		
		///not thread-safe
		std::vector<BlobDataBuffer> getNextBlocks(uint32_t num);
		
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
		BlobFileIn * m_FileIn;
		BlobDataBuffer m_DataBuffer;

		crosby::binary::HeaderBlock * m_FileHeader;
		std::vector< bool > m_MissingFeatures;

		OffsetType m_DataOffset;

		bool parseHeader();

	private:
		OSMFileIn();
		OSMFileIn(const OSMFileIn & other);
		OSMFileIn & operator=(const OSMFileIn & other);
	};

	// TODO
	class OSMFileOut;
}

#endif // OSMPBF_OSMFILE_H
