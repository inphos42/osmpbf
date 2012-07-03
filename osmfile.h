#ifndef OSMPBF_OSMFILE_H
#define OSMPBF_OSMFILE_H

#include <string>

#include "blobdata.h"

class HeaderBlock;

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

		void dataSeek(uint32_t position);
		uint32_t dataPosition() const;
		uint32_t dataSize() const;

		uint32_t totalSize() const;

		bool parseBlock(PrimitiveBlockInputAdaptor & adaptor);
		bool skipBlock();
		bool readBlock();

		const BlobDataBuffer & blockBuffer() const { return m_DataBuffer; }

		bool parserMeetsRequirements() const;

		int requiredFeaturesSize() const;
		int optionalFeaturesSize() const;

		const std::string & requiredFeatures(int index) const;
		const std::string & optionalFeatures(int index) const;

		// bounding box in nanodegrees
		int64_t minLat() const;
		int64_t maxLat() const;

		int64_t minLon() const;
		int64_t maxLon() const;

	protected:
		BlobFileIn * m_FileIn;
		BlobDataBuffer m_DataBuffer;

		HeaderBlock * m_FileHeader;

		uint32_t m_DataOffset;

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
