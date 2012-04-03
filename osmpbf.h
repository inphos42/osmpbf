#ifndef osmpbf_H
#define osmpbf_H

#include <string>

class osmpbf {
public:
	osmpbf(std::string fileName);
	virtual ~osmpbf();
	
	void open();
	void close();
	
	bool readBlob();
private:
	enum BlobDataType {BLOB_Invalid = 0, BLOB_OSMHeader = 1, BLOB_OSMData = 2};
	
	std::string m_FileName;
	std::fstream * m_Stream;
	
	bool readOSMHeader(const char * rawdata, uint32_t size);
	bool readOSMData(const char * rawdata, uint32_t size);
};

#endif // osmpbf_H
