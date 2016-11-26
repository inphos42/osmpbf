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

#include <osmpbf/blobfile.h>
#include <osmpbf/fileio.h>
#include <osmpbf/net.h>

#include "osmblob.pb.h"

#include <iostream>
#include <limits>
#include <zlib.h>
#include <assert.h>

namespace osmpbf
{

bool inflateData(const char * source, uint32_t sourceSize, char * dest, uint32_t destSize)
{
	int ret;
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

	ret = inflate(&stream, Z_FINISH);

	assert(ret != Z_STREAM_ERROR);

	switch (ret)
	{
	case Z_NEED_DICT:
		std::cerr << "ERROR: zlib - Z_NEED_DICT" << std::endl;
		inflateEnd(&stream);
		return false;
	case Z_DATA_ERROR:
		std::cerr << "ERROR: zlib - Z_DATA_ERROR" << std::endl;
		inflateEnd(&stream);
		return false;
	case Z_MEM_ERROR:
		std::cerr << "ERROR: zlib - Z_MEM_ERROR" << std::endl;
		inflateEnd(&stream);
		return false;
	default:
		break;
	}

	inflateEnd(&stream);
	return true;
}

uint32_t deflateData(const char * source, uint32_t sourceSize, char *& dest, uint32_t & destSize)
{
	int ret;
	z_stream stream;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;

	ret = deflateInit(&stream, Z_BEST_COMPRESSION);
	assert(ret != Z_STREAM_ERROR);

	//overvlow will result in error during stream decoding
	destSize = (uint32_t) deflateBound(&stream, sourceSize);
	dest = new char[destSize];

	stream.avail_in = sourceSize;
	stream.next_in = (Bytef *)source;
	stream.avail_out = destSize;
	stream.next_out = (Bytef *)dest;

	ret = deflate(&stream, Z_FINISH);

	assert(ret != Z_STREAM_ERROR);

	deflateEnd(&stream);
	switch (ret)
	{
	case Z_NEED_DICT:
		std::cerr << "ERROR: zlib - Z_NEED_DICT" << std::endl;
		return 0;
	case Z_DATA_ERROR:
		std::cerr << "ERROR: zlib - Z_DATA_ERROR" << std::endl;
		return 0;
	case Z_MEM_ERROR:
		std::cerr << "ERROR: zlib - Z_MEM_ERROR" << std::endl;
		return 0;
	case Z_STREAM_END:
		//has to smaller than destSize (which is uint32_t)
		return (uint32_t) stream.total_out;
	default:
		std::cerr << "ERROR: zlib - input not compressable" << std::endl;
		return 0;
	}
}

AbstractBlobFile::AbstractBlobFile(const std::string & fileName)
	: m_FileName(fileName),
	  m_FileDescriptor(-1),
	  m_VerboseOutput(false)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

BlobFileIn::BlobFileIn(const std::string & fileName)
	: AbstractBlobFile(fileName), m_FileData(NULL)
{
}

BlobFileIn::~BlobFileIn()
{
	close();
}

bool BlobFileIn::open()
{
	close();

	if (m_VerboseOutput) std::cout << "opening File " << m_FileName << " ...";

	m_FileData = NULL;
	m_FileSize = 0;
	m_FilePos = 0;

	m_FileDescriptor = osmpbf::open(m_FileName.c_str(), IO_OPEN_READ_ONLY);
	if (m_FileDescriptor < 0) {
		std::cerr << "ERROR: Could not open file: " << m_FileName << std::endl;
		return false;
	}
	
	uint64_t fileSize = osmpbf::fileSize(m_FileDescriptor);
	if (!fileSize) {
		std::cerr << "ERROR: File is empty or non-existent" << std::endl;
		osmpbf::close(m_FileDescriptor);
		m_FileDescriptor = -1;
		return false;
	}
	
	if (fileSize > (std::numeric_limits<SizeType>::max)())
	{
		std::cerr << "ERROR: input file is larger than " << ((std::numeric_limits<SizeType>::max)() >> 30) << " GiB" << std::endl;
		osmpbf::close(m_FileDescriptor);
		m_FileDescriptor = -1;
		return false;
	}

	m_FileSize = SizeType(fileSize);

	m_FileData = (char *) mmap(0, m_FileSize, MM_PROT_READ, MM_MAP_SHARED, m_FileDescriptor, 0);

	if (!osmpbf::validMmapAddress(m_FileData))
	{
		std::cerr << "ERROR: could not mmap file" << std::endl;
		osmpbf::close(m_FileDescriptor);
		m_FileDescriptor = -1;
		m_FileData = NULL;
		return false;
	}

	if (m_VerboseOutput) std::cout << "done" << std::endl;
	return true;
}

void BlobFileIn::close()
{
	if (m_FileData)
	{
		if (m_VerboseOutput) std::cout << "closing file ...";
		osmpbf::munmap(m_FileData, m_FileSize);
		osmpbf::close(m_FileDescriptor);
		if (m_VerboseOutput) std::cout << "done" << std::endl;

		m_FileData = NULL;
	}
}

void BlobFileIn::seek(OffsetType position)
{
	m_FilePos = position;
}

SizeType BlobFileIn::position() const
{
	return m_FilePos;
}

SizeType BlobFileIn::size() const
{
	return m_FileSize;
}

void BlobFileIn::readBlob(BlobDataBuffer & buffer)
{
	buffer.type = readBlob(buffer.data, buffer.totalBytes, buffer.availableBytes);
}

constexpr uint32_t MAX_HEADER_SIZE = 64 << 10;
constexpr uint32_t MAX_BODY_SIZE = 32 << 20;

void BlobFileIn::readBlobHeader(uint32_t & blobLength, osmpbf::BlobDataType & blobDataType)
{
	blobDataType = BLOB_Invalid;

	if (m_VerboseOutput) std::cout << "checking blob header ..." << std::endl;

	uint32_t headerLength = osmpbf::net2hostLong(* (uint32_t *) fileData());

	if (m_VerboseOutput) std::cout << "header length : " << headerLength << " B" << std::endl;

	if (!headerLength || headerLength >= MAX_HEADER_SIZE)
	{
		std::cerr << "ERROR: invalid blob header size found:" << headerLength;
		if (headerLength >= MAX_HEADER_SIZE)
			std::cerr << " (max: " << MAX_HEADER_SIZE << ')';

		std::cerr << std::endl;
		return;
	}

	m_FilePos += 4;

	if (m_VerboseOutput) std::cout << "parsing blob header ..." << std::endl;

	BlobHeader * blobHeader = new BlobHeader();

	if (!blobHeader->ParseFromArray(fileData(), headerLength))
	{
		std::cerr << "ERROR: invalid blob header structure" << std::endl;

		if (!blobHeader->has_type())
			std::cerr << "> no \"type\" field found" << std::endl;

		if (!blobHeader->has_datasize())
			std::cerr << "> no \"datasize\" field found" << std::endl;
	}
	else
	{
		m_FilePos += headerLength;

		if (m_VerboseOutput) std::cout << "type : " << blobHeader->type() << std::endl;
		if (m_VerboseOutput) std::cout << "datasize : " << blobHeader->datasize() << " B ( " << blobHeader->datasize() / 1024.f << " KiB )" << std::endl;

		if (blobHeader->type() == "OSMHeader")
			blobDataType = BLOB_OSMHeader;
		else if (blobHeader->type() == "OSMData")
			blobDataType = BLOB_OSMData;

		blobLength = blobHeader->datasize();
	}

	delete blobHeader;
}

BlobDataType BlobFileIn::readBlob(char * & buffer, uint32_t & bufferSize, uint32_t & availableDataSize)
{
	if (m_FilePos >= m_FileSize)
		return BLOB_Invalid;

	if (m_VerboseOutput) std::cout << "== blob ==" << std::endl;

	uint32_t blobLength;
	BlobDataType blobDataType;

	readBlobHeader(blobLength, blobDataType);

	if (blobLength >= MAX_BODY_SIZE)
	{
		std::cerr << "ERROR: invalid blob size found:" << blobLength << " (max: " << MAX_BODY_SIZE << ')' << std::endl;
		return BLOB_Invalid;
	}

	if (blobDataType && blobLength)
	{
		if (m_VerboseOutput) std::cout << "parsing blob ..." << std::endl;

		Blob * blob = new Blob();

		if (!blob->ParseFromArray(fileData(), blobLength))
		{
			std::cerr << "ERROR: invalid blob structure" << std::endl;

			delete blob;
			return BLOB_Invalid;
		}

		m_FilePos += blobLength;

		if (blob->has_raw_size())
		{
			if (m_VerboseOutput) std::cout << "found compressed blob data" << std::endl;
			if (m_VerboseOutput) std::cout << "uncompressed size : " << blob->raw_size() << "B ( " << blob->raw_size() / 1024.f << " KiB )" << std::endl;

			std::string * compressedData = blob->release_zlib_data();
			availableDataSize = blob->raw_size();

			delete blob;

			if (bufferSize < availableDataSize)
			{
				if (buffer) delete[] buffer;
				buffer = new char[availableDataSize];

				bufferSize = availableDataSize;
			}

			if (m_VerboseOutput) std::cout << "decompressing data ... ";
			
			assert(compressedData->length() < std::numeric_limits<uint32_t>::max());
			inflateData(compressedData->data(), (uint32_t) compressedData->length(), buffer, availableDataSize);

			if (m_VerboseOutput) std::cout << "done" << std::endl;

			delete compressedData;
		}
		else
		{
			if (m_VerboseOutput) std::cout << "found uncompressed blob data" << std::endl;

			std::string * uncompressedData = blob->release_raw();
			assert(uncompressedData->length() < std::numeric_limits<uint32_t>::max());
			availableDataSize = (uint32_t) uncompressedData->length();

			delete blob;

			if (bufferSize < availableDataSize)
			{
				if (buffer) delete[] buffer;
				buffer = new char[availableDataSize];

				bufferSize = availableDataSize;
			}

			memmove(buffer, uncompressedData->data(), availableDataSize);
			delete uncompressedData;
		}

		return blobDataType;
	}

	if (!blobDataType)
		std::cerr << "ERROR: invalid blob type" << std::endl;
	if (!blobLength)
		std::cerr << "ERROR: invalid blob size" << std::endl;

	return BLOB_Invalid;
}

bool BlobFileIn::skipBlob()
{
	if (m_FilePos >= m_FileSize)
		return false;

	if (m_VerboseOutput) std::cout << "== blob ==" << std::endl;

	uint32_t blobLength;
	BlobDataType blobDataType;

	readBlobHeader(blobLength, blobDataType);
	if (blobLength)
	{
		if (m_VerboseOutput) std::cout << "skipping blob" << std::endl;

		m_FilePos += blobLength;
		return true;
	}
	else
	{
		std::cerr << "ERROR: invalid blob size" << std::endl;
		return false;
	}
}


BlobFileOut::BlobFileOut(const std::string & fileName)
	: AbstractBlobFile(fileName), m_CurrentSize(0)
{
}

BlobFileOut::~BlobFileOut()
{
	close();
}

bool BlobFileOut::open()
{
	if (m_VerboseOutput) std::cout << "opening/creating File " << m_FileName << " ...";

	m_FileDescriptor = osmpbf::open(m_FileName.c_str(), IO_OPEN_WRITE_ONLY | IO_OPEN_CREATE | IO_OPEN_TRUNCATE, 0666);

	if (m_VerboseOutput) std::cout << "done" << std::endl;

	return m_FileDescriptor > -1;
}

void BlobFileOut::close()
{
	if (m_FileDescriptor > -1)
	{
		if (m_VerboseOutput) std::cout << "closing File " << m_FileName << " ...";
		osmpbf::close(m_FileDescriptor);
		if (m_VerboseOutput) std::cout << "done" << std::endl;
	}
}

void BlobFileOut::seek(OffsetType position)
{
	osmpbf::lseek(m_FileDescriptor, position, IO_SEEK_SET);
}

SizeType BlobFileOut::position() const
{
	return osmpbf::lseek(m_FileDescriptor, 0, IO_SEEK_CUR);
}

SizeType BlobFileOut::size() const
{
	return m_CurrentSize;
}

bool BlobFileOut::writeBlob(const BlobDataBuffer & buffer, bool compress)
{
	return writeBlob(buffer.type, buffer.data, buffer.availableBytes, compress);
}

bool BlobFileOut::writeBlob(osmpbf::BlobDataType type, const char * buffer, uint32_t bufferSize, bool compress)
{
	if (type == BLOB_Invalid)
		return false;

	if (m_VerboseOutput) std::cout << "preparing blob data:" << std::endl;
	Blob * blob = new Blob();

	if (compress)
	{
		char * zlibBuffer = NULL;
		uint32_t zlibBufferSize = 0;
		uint32_t zlibDataAvailable = 0;

		if (m_VerboseOutput) std::cout << "compressing data ... ";
		zlibDataAvailable = deflateData(buffer, bufferSize, zlibBuffer, zlibBufferSize);
		if (m_VerboseOutput) std::cout << "done" << std::endl;

		blob->set_raw_size(bufferSize);
		blob->set_zlib_data((void *)zlibBuffer, zlibDataAvailable);
		delete[] zlibBuffer;
	}
	else
	{
		if (m_VerboseOutput) std::cout << " <no compression requested>" << std::endl;
		blob->set_raw((void *)buffer, bufferSize);
	}

	if (!blob->IsInitialized())
	{
		std::cerr << "blob not initialized" << std::endl;
		return false;
	}

	std::string serializedBlobBuffer = blob->SerializeAsString();
	delete blob;

	if (!serializedBlobBuffer.length())
	{
		std::cerr << "serializedBlobBuffer failed" << std::endl;
		return false;
	}

	BlobHeader * blobHeader = new BlobHeader();
	assert(serializedBlobBuffer.length() < std::numeric_limits<::google::protobuf::int32>::max());
	blobHeader->set_datasize((::google::protobuf::int32) serializedBlobBuffer.length());
	switch (type)
	{
	case BLOB_OSMData:
		blobHeader->set_type("OSMData");
		break;
	case BLOB_OSMHeader:
		blobHeader->set_type("OSMHeader");
		break;
	default:
		break;
	}

	if (!blobHeader->IsInitialized())
	{
		std::cerr << "blobHeader not initialized" << std::endl;
		delete blobHeader;
		return false;
	}

	if (m_VerboseOutput) std::cout << "writing blob...";

	// save position and skip header size
	off_t headerSizePosition = osmpbf::lseek(m_FileDescriptor, 0, IO_SEEK_CUR);
	osmpbf::lseek(m_FileDescriptor, sizeof(uint32_t), IO_SEEK_CUR);

	// serialize header blob
	if (!blobHeader->SerializeToFileDescriptor(m_FileDescriptor))
	{
		std::cerr << "error writing blobHeader" << std::endl;
		delete blobHeader;
		return false;
	}

	off_t blobPosition = osmpbf::lseek(m_FileDescriptor, 0, IO_SEEK_CUR);

	// write header size
	uint32_t headerSize = blobHeader->ByteSize();
	headerSize = osmpbf::host2NetLong(headerSize);
	osmpbf::lseek(m_FileDescriptor, headerSizePosition, IO_SEEK_SET);
	osmpbf::write(m_FileDescriptor, &headerSize, sizeof(uint32_t));
	osmpbf::lseek(m_FileDescriptor, blobPosition, IO_SEEK_SET);

	delete blobHeader;

	// write blob
	osmpbf::write(m_FileDescriptor, (void *) serializedBlobBuffer.data(), serializedBlobBuffer.length());
	if (m_VerboseOutput) std::cout << "done" << std::endl;

	SizeType pos = position();
	if (m_CurrentSize < pos)
		m_CurrentSize = pos;

	return true;
}

} // namespace osmpbf
