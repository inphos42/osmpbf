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

#ifndef OSMPBF_BLOBDATA_H
#define OSMPBF_BLOBDATA_H

#include <cstddef>
#include <cstdint>
#include <cstring>

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
			type = BLOB_Invalid;
		}

		BlobDataBuffer() : type(BLOB_Invalid), data(0), availableBytes(0), totalBytes(0) {}
		BlobDataBuffer(const BlobDataBuffer & other) :
			type(BLOB_Invalid), data(NULL),
			availableBytes(other.availableBytes), totalBytes(other.availableBytes)
		{
			if (totalBytes) {
				data = new char[totalBytes];
				memmove(data, other.data, totalBytes);
				type = other.type;
			}
		}

		BlobDataBuffer(BlobDataBuffer && other) :
			type(other.type), data(other.data),
			availableBytes(other.availableBytes), totalBytes(other.availableBytes)
		{
			other.type = BLOB_Invalid;
			other.data = 0;
			other.availableBytes = 0;
			other.totalBytes = 0;
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

		BlobDataBuffer & operator=(BlobDataBuffer && other) {
			clear();

			availableBytes = other.availableBytes;
			totalBytes = other.totalBytes;
			type = other.type;
			data = other.data;

			other.data = NULL;
			other.availableBytes = 0;
			other.totalBytes = 0;
			other.type = BLOB_Invalid;

			return *this;
		}
	};
}

#endif // OSMPBF_BLOBDATA_H
