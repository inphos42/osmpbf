#ifndef OSMPBF_BLOBDATA_H
#define OSMPBF_BLOBDATA_H

#include <cstdint>

namespace osmpbf {
	enum BlobDataType {BLOB_Invalid = 0, BLOB_OSMHeader = 1, BLOB_OSMData = 2};

	struct BlobDataBuffer {
		BlobDataType type;
		char * data;
		uint32_t availableBytes;
		uint32_t totalBytes;

		inline void clear() {
			delete[] data;
			data = NULL;
			availableBytes = 0;
			totalBytes = 0;
		}

		BlobDataBuffer() : type(BLOB_Invalid), data(NULL), availableBytes(0), totalBytes(0) {}
		~BlobDataBuffer() { clear(); }
	};
}

#endif
