#ifndef OSMPBF_BLOBDATA_H
#define OSMPBF_BLOBDATA_H

#include <cstdint>
#include <cstring>
#include <typelimits.h>

namespace osmpbf {
	enum BlobDataType {BLOB_Invalid = 0, BLOB_OSMHeader = 1, BLOB_OSMData = 2};

	struct BlobDataBuffer {
		BlobDataType type;
		char * data;
		OffsetType availableBytes;
		OffsetType totalBytes;

		inline void clear() {
			delete[] data;
			data = nullptr;
			availableBytes = 0;
			totalBytes = 0;
			type = BLOB_Invalid;
		}

		BlobDataBuffer() : type(BLOB_Invalid), data(nullptr), availableBytes(0), totalBytes(0) {}
		BlobDataBuffer(const BlobDataBuffer & other) :
			type(BLOB_Invalid), data(nullptr),
			availableBytes(other.availableBytes), totalBytes(other.availableBytes)
		{
			if (totalBytes) {
				data = new char[totalBytes];
				memmove(data, other.data, totalBytes);
				type = other.type;
			}
		}

		~BlobDataBuffer() { clear(); }

		BlobDataBuffer & operator=(const BlobDataBuffer & other) {
			clear();

			availableBytes = other.availableBytes;
			totalBytes = availableBytes;

			if (totalBytes) {
				data = new char[totalBytes];
				memmove(data, other.data, totalBytes);
				type = other.type;
			}

			return *this;
		}
	};
}

#endif
