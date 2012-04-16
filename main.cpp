#include "osmblobfile.h"
#include "osmdatacontroller.h"
#include "osmpbf.h"
#include <iostream>
#include <cstdint>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage:  " << argv[0] << " FILE_NAME" << std::endl;
		return -1;
	}
	
	osmpbf::OSMBlobFile foo(argv[1]);
	
	char * buffer = NULL;
	uint32_t bufferLength = 0;
	
	osmpbf::OSMBlobFile::BlobDataType blobType = osmpbf::OSMBlobFile::BLOB_Invalid;
	
	foo.open();
	do {
		blobType = foo.nextBlobData(buffer, bufferLength);
		switch (blobType) {
		case osmpbf::OSMBlobFile::BLOB_OSMHeader:
			OSMpbf::readOSMHeader(buffer, bufferLength);
			break;
		case osmpbf::OSMBlobFile::BLOB_OSMData:
			{
				osmpbf::OSMPrimitiveBlockController pbc(buffer, bufferLength);
				if (pbc.isNull()) {
					std::cerr << "created OSMPrimitiveBlockController is null" << std::endl;
					break;
				}
				
				std::cout << "nodesSize = " << pbc.nodesSize() << std::endl;
				std::cout << "waysSize = " << pbc.waysSize() << std::endl;
			}
			break;
		default: break;
		}
		
		if (buffer) delete[] buffer;
		buffer = NULL;
		bufferLength = 0;
	}
	while (blobType);
	
	foo.close();
	
	
	
	return 0;
}
