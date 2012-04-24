#include "osmblobfile.h"

#include <iostream>
#include <netinet/in.h>
#include <zlib.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "osmblob.pb.h"

using namespace std;

namespace osmpbf {
	bool inflateData(const char * source, uint32_t sourceSize, char * dest, uint32_t destSize) {
		cout << "decompressing data ... ";
		int ret;
		unsigned int have;
		z_stream stream;

		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		stream.avail_in = sourceSize;
		stream.next_in = (Bytef *)source;
		stream.avail_out = destSize;
		stream.next_out = (Bytef *)dest;

		ret = inflateInit(&stream);
		if (ret != Z_OK)
			return false;

		ret = inflate(&stream, Z_NO_FLUSH);

		assert(ret != Z_STREAM_ERROR);

		switch (ret) {
		case Z_NEED_DICT:
			cerr << "ERROR: zlib - Z_NEED_DICT" << endl;
			inflateEnd(&stream);
			return false;
		case Z_DATA_ERROR:
			cerr << "ERROR: zlib - Z_DATA_ERROR" << endl;
			inflateEnd(&stream);
			return false;
		case Z_MEM_ERROR:
			cerr << "ERROR: zlib - Z_MEM_ERROR" << endl;
			inflateEnd(&stream);
			return false;
		default:
			break;
		}
		cout << "done" << endl;

		inflateEnd(&stream);
		return true;
	}

	OSMBlobFile::OSMBlobFile(std::string fileName) : m_FileName(fileName), m_FileDescriptor(-1), m_FileData(NULL) {
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	OSMBlobFile::~OSMBlobFile() {
		close();
	}

	bool OSMBlobFile::open() {
		close();

		std::cout << "opening File " << m_FileName << " ...";

		m_FileData = NULL;
		m_FileSize = 0;
		m_FilePos = 0;

		m_FileDescriptor = ::open(m_FileName.c_str(), O_RDONLY);
		if (m_FileDescriptor < 0) return false;

		struct stat stFileInfo;
		if (fstat(m_FileDescriptor, &stFileInfo) == 0) {
			if (stFileInfo.st_size > INT32_MAX) {
				cerr << "ERROR: input file is larger than 4GB" << endl;
				::close(m_FileDescriptor);
				m_FileDescriptor = -1;
				return false;
			}

			m_FileSize = uint32_t(stFileInfo.st_size);
		}

		m_FileData = (char *) mmap(0, m_FileSize, PROT_READ, MAP_SHARED, m_FileDescriptor, 0);

		if ((void *) m_FileData == MAP_FAILED) {
			cerr << "ERROR: could not mmap file" << endl;
			::close(m_FileDescriptor);
			m_FileDescriptor = -1;
			m_FileData = NULL;
			return false;
		}

		std::cout << "done" << std::endl;
	}

	void OSMBlobFile::close() {
		if (m_FileData) {
			std::cout << "closing file ...";
			munmap(m_FileData, m_FileSize);
			::close(m_FileDescriptor);
			std::cout << "done" << std::endl;

			m_FileData = NULL;
		}
	}

	void OSMBlobFile::reset() {
		open();
	}

	OSMBlobFile::BlobDataType OSMBlobFile::readBlobData(char * & buffer, uint32_t & bufferLength) {
		if (m_FilePos > m_FileSize - 1)
			return BLOB_Invalid;

		buffer = NULL;
		bufferLength = 0;

		cout << "== blob ==" << endl;
		BlobDataType blobDataType = BLOB_Invalid;

		cout << "checking blob header ..." << endl;

		uint32_t blobLength;
		uint32_t headerLength = ntohl(* (uint32_t *) fileData());

		cout << "header length : " << headerLength << " B" << endl;

		if (!headerLength)
			return BLOB_Invalid;

		m_FilePos += 4;

		cout << "parsing blob header ..." << endl;
		{
			BlobHeader * blobHeader = new BlobHeader();

			if (!blobHeader->ParseFromArray(fileData(), headerLength)) {
				cerr << "ERROR: invalid blob header structure" << endl;

				if (!blobHeader->has_type())
					cerr << "> no \"type\" field found" << endl;

				if (!blobHeader->has_datasize())
					cerr << "> no \"datasize\" field found" << endl;

				delete blobHeader;

				return BLOB_Invalid;
			}

			m_FilePos += headerLength;

			cout << "type : " << blobHeader->type() << endl;
			cout << "datasize : " << blobHeader->datasize() << " B ( " << blobHeader->datasize() / 1024.f << " KiB )" << endl;

			if (blobHeader->type() == "OSMHeader")
				blobDataType = BLOB_OSMHeader;
			else if (blobHeader->type() == "OSMData")
				blobDataType = BLOB_OSMData;

			blobLength = blobHeader->datasize();
			delete blobHeader;
		}

		if (blobDataType && blobLength) {
			cout << "parsing blob ..." << endl;

			Blob * blob = new Blob();

			if (!blob->ParseFromArray(fileData(), blobLength)) {
				cerr << "error: invalid blob structure" << endl;

				delete blob;

				return BLOB_Invalid;
			}

			m_FilePos += blobLength;

			if (blob->has_raw_size()) {
				cout << "found compressed blob data" << endl;
				cout << "uncompressed size : " << blob->raw_size() << "B ( " << blob->raw_size() / 1024.f << " KiB )" << endl;

				string * compressedData = blob->release_zlib_data();
				bufferLength = blob->raw_size();

				delete blob;

				buffer = new char[bufferLength];

				inflateData(compressedData->data(), compressedData->length(), buffer, bufferLength);
				delete compressedData;
			}
			else {
				cout << "found uncompressed blob data" << endl;

				string * uncompressedData = blob->release_raw();
				bufferLength = uncompressedData->length();

				delete blob;

				buffer = new char[bufferLength];

				memmove(buffer, uncompressedData->data(), bufferLength);
				delete uncompressedData;
			}

			return blobDataType;
		}

		cerr << "ERROR: ";
		if (!blobDataType)
			cerr << "invalid blob type";
		if (!bufferLength)
			cerr << "invalid blob size";
		cerr << endl;

		return BLOB_Invalid;
	}
}
