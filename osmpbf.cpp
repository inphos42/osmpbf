#include "osmpbf.h"

#include <iostream>
#include <fstream>

#include "osmformat.pb.h"
#include "osmblob.pb.h"
#include <cstdint>
#include <netinet/in.h>
#include <zlib.h>

using namespace std;

osmpbf::osmpbf(std::string fileName) : m_FileName(fileName), m_Stream(NULL) {
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

osmpbf::~osmpbf() {
	close();
}

void osmpbf::open() {
	close();
	
	HeaderBlock header;
	
	std::cout << "opening File " << m_FileName << " ...";
	m_Stream = new std::fstream(m_FileName.data(), std::ios::in | std::ios::binary);
	std::cout << "done" << std::endl;
}

void osmpbf::close() {
	if (m_Stream) {
		std::cout << "closing file ...";
		delete m_Stream;
		m_Stream = NULL;
		std::cout << "done" << std::endl;
	}
}


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
	
	return true;
}

bool osmpbf::readBlob() {
	char * buffer = NULL;
	uint32_t bufferLength = 0;
	cout << "== blob ==" << endl;
	BlobDataType blobDataType = BLOB_Invalid;
	
	cout << "checking blob header ..." << endl;
	m_Stream->read((char*)&bufferLength, 4);
	
	bufferLength = ntohl(bufferLength);
	cout << "header length : " << bufferLength << " B" << endl;
	
	if (!bufferLength) {
		delete[] buffer;
		return false;
	}
	
	cout << "parsing blob header ..." << endl;
	
	buffer = new char[bufferLength];
	m_Stream->read(buffer, bufferLength);
	
	{
		BlobHeader * blobHeader = new BlobHeader();
		
		if (!blobHeader->ParseFromArray((void *)buffer, bufferLength)) {
			cerr << "ERROR: invalid blob header structure" << endl;
			
			if (!blobHeader->has_type())
				cerr << "> no \"type\" field found" << endl;
			
			if (!blobHeader->has_datasize())
				cerr << "> no \"datasize\" field found" << endl;
			
			if (buffer)
				delete[] buffer;
			
			delete blobHeader;
			
			return false;
		}
		
		if (buffer)
			delete[] buffer;
		
		cout << "type : " << blobHeader->type() << endl;
		cout << "datasize : " << blobHeader->datasize() << " B ( " << blobHeader->datasize() / 1024.f << " KiB )" << endl;
		
		if (blobHeader->type() == "OSMHeader")
			blobDataType = BLOB_OSMHeader;
		else if (blobHeader->type() == "OSMData")
			blobDataType = BLOB_OSMData;
		
		bufferLength = blobHeader->datasize();
		delete blobHeader;
	}
	
	bool result = false;
	if (blobDataType && bufferLength) {
		cout << "parsing blob ..." << endl;
		
		buffer = new char[bufferLength];
		m_Stream->read(buffer, bufferLength);
		
		Blob * blob = new Blob();
		
		if (!blob->ParseFromArray((void *)buffer, bufferLength)) {
			cerr << "error: invalid blob structure" << endl;
			
			if (buffer)
				delete[] buffer;
			
			return false;
		}
		
		if (blob->has_raw_size()) {
			cout << "found compressed blob data" << endl;
			cout << "uncompressed size : " << blob->raw_size() << "B ( " << blob->raw_size() / 1024.f << " KiB )" << endl;
			
			bufferLength = blob->raw_size();
			string * compressedData = blob->release_zlib_data();
			
			delete blob;
			delete[] buffer;
			
			buffer = new char[bufferLength];
			
			inflateData(compressedData->data(), compressedData->length(), buffer, bufferLength);
			delete compressedData;
			
			
			switch (blobDataType) {
				case BLOB_OSMHeader: result = readOSMHeader(buffer, bufferLength); break;
				case BLOB_OSMData: result = readOSMData(buffer, bufferLength); break;
				default: break;
			}
			delete[] buffer;
			
		}
		else {
			cout << "found uncompressed blob data" << endl;
			
			string * uncompressedData = blob->release_raw();
			delete blob;
			delete[] buffer;
			
			switch (blobDataType) {
				case BLOB_OSMHeader: result = readOSMHeader(uncompressedData->data(), uncompressedData->length()); break;
				case BLOB_OSMData: result = readOSMData(uncompressedData->data(), uncompressedData->length()); break;
				default: break;
			}
			delete uncompressedData;
		}
		
		buffer = NULL;
	}
	else {
		cerr << "ERROR: ";
		if (!blobDataType)
			cerr << "invalid blob type";
		if (!bufferLength)
			cerr << "invalid blob size";
		cerr << endl;
		
		result = false;
	}
	
	if (buffer)
		delete[] buffer;
	
	return result;
}

bool osmpbf::readOSMHeader(const char * rawdata, uint32_t size) {
	cout << "--- OSM header ---" << endl;
	
	HeaderBlock headerBlock;
	if (!headerBlock.ParseFromArray((void *)rawdata, size)) {
		cerr << "ERROR: invalid OSM header block" << endl;
		return false;
	}
	
	cout << "required features:" << endl;
	for (int i = 0; i < headerBlock.required_features_size(); i++ ) {
		cout << ' ' << headerBlock.required_features(i) << endl;
	}
	
	cout << "optional features:" << endl;
	if (headerBlock.optional_features_size())
		for (int i = 0; i < headerBlock.optional_features_size(); i++ ) {
			cout << ' ' << headerBlock.optional_features(i) << endl;
		}
	else
		cout << " <none>" << endl;
	
	if (headerBlock.has_bbox()) {
		cout << "found bounding box" << endl;
	}
	
	// not handling optional writingprogram
	
	cout << "--- OSM header ---" << endl;
	
	return true;
}

bool osmpbf::readOSMData(const char * rawdata, uint32_t size) {
	cout << "--- OSM data ---" << endl;
	
	PrimitiveBlock primitiveBlock;
	if (!primitiveBlock.ParseFromArray((void*)rawdata, size)) {
		cerr << "ERROR: invalid OSM data block" << endl;
		return false;
	}
	
	google::protobuf::RepeatedPtrField< PrimitiveGroup >::const_iterator it;
	for (it = primitiveBlock.primitivegroup().begin(); it != primitiveBlock.primitivegroup().end(); ++it) {
		cout << "found primitive group:" << endl;
		cout << it->nodes_size() << " nodes" << endl;
		if (it->has_dense()) {
			cout << it->dense().id_size() << " dense node ids" << endl;
		}
		cout << it->ways_size() << " ways" << endl;
		cout << it->relations_size() << " relations" << endl;
	}
	
	cout << "--- OSM data ---" << endl;
	return true;
}
