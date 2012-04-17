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

		if (blobType == osmpbf::OSMBlobFile::BLOB_OSMData) {
			osmpbf::OSMPrimitiveBlockController pbc(buffer, bufferLength);

			if (pbc.nodesSize() > 10) {
				for (int n = 0; n < 10; n++) {
					osmpbf::OSMNode node = pbc.getNodeAt(n);

					std::cout << "node " << n << ':' << std::endl;
					std::cout << "id = " << node.id() << std::endl;
					std::cout << "lat = " << node.lat() << std::endl;
					std::cout << "lon = " << node.lon() << std::endl;
					std::cout << "keys, vals:" << std::endl;

					if (node.keysSize())
						for (int i = 0; i < node.keysSize(); i++)
							std::cout << '[' << i << "] " << node.key(i) << " = " << node.value(i) << std::endl;
					else
						std::cout << " <no keys>" << std::endl;
				}
				break;
			}
		}

// 		switch (blobType) {
// 		case osmpbf::OSMBlobFile::BLOB_OSMHeader:
// 			OSMpbf::readOSMHeader(buffer, bufferLength);
// 			break;
// 		case osmpbf::OSMBlobFile::BLOB_OSMData:
// 			{
// 				osmpbf::OSMPrimitiveBlockController pbc(buffer, bufferLength);
// 				if (pbc.isNull()) {
// 					std::cerr << "created OSMPrimitiveBlockController is null" << std::endl;
// 					break;
// 				}
//
// 				std::cout << "nodesSize = " << pbc.nodesSize() << std::endl;
// 				std::cout << "waysSize = " << pbc.waysSize() << std::endl;
//
// 				osmpbf::OSMNode node = pbc.getNodeAt(10);
// 				if (!node.isNull())
// 					std::cout << node.lat() << std::endl;
// 			}
// 			break;
// 		default: break;
// 		}

		// cleanup
		if (buffer) delete[] buffer;
		buffer = NULL;
		bufferLength = 0;
	}
	while (blobType);

	foo.close();

	return 0;
}



