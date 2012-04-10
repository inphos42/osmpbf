#include "osmpbf.h"
#include <iostream>

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "Usage:  " << argv[0] << " FILE_NAME" << std::endl;
		return -1;
	}
	
	osmpbf foo(argv[1]);
	
	foo.open();
	while (foo.readBlob()) ;
	foo.close();
	
	return 0;
}
