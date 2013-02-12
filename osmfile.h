#ifndef OSMPBF_OSMFILE_H
#define OSMPBF_OSMFILE_H

#include <string>
#include <vector>

#include "blobdata.h"

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

#endif
