#ifndef osmpbf_H
#define osmpbf_H

#include <string>

class OSMpbf {
public:
	OSMpbf(std::string fileName);
	virtual ~OSMpbf();
	
	void open();
	void close();
	
	bool readBlob();
	
	static bool readOSMHeader(const char * rawdata, uint32_t size);
	static bool readOSMData(const char * rawdata, uint32_t size);
private:
	enum BlobDataType {BLOB_Invalid = 0, BLOB_OSMHeader = 1, BLOB_OSMData = 2};
	
	std::string m_FileName;
	std::fstream * m_Stream;
};

#endif // osmpbf_H
