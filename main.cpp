#include "osmblobfile.h"
#include "osmpbf.h"
#include <iostream>
#include <cstdint>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage:  " << argv[0] << " FILE_NAME" << std::endl;
		return -1;
	}
	
	osmpbf::OSMBlobFile foo(argv[1]);
	
	char * buffer;
	uint32_t bufferLength;
	
	osmpbf::OSMBlobFile::BlobDataType blobType = osmpbf::OSMBlobFile::BLOB_Invalid;
	
	foo.open();
	do {
		blobType = foo.nextBlobData(buffer, bufferLength);
		switch (blobType) {
		case osmpbf::OSMBlobFile::BLOB_OSMHeader:
			OSMpbf::readOSMHeader(buffer, bufferLength);
			break;
		case osmpbf::OSMBlobFile::BLOB_OSMData:
			OSMpbf::readOSMData(buffer, bufferLength);
			break;
		default: break;
		}
	}
	while (blobType);
	
	foo.close();
	
	return 0;
}
