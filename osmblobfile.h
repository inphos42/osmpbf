#ifndef OSMPBF_OSMBLOBFILE_H
#define OSMPBF_OSMBLOBFILE_H

#include <cstdint>
#include <string>

namespace osmpbf {
	class OSMBlobFile {
	public:
		enum BlobDataType {BLOB_Invalid = 0, BLOB_OSMHeader = 1, BLOB_OSMData = 2};

		OSMBlobFile(std::string fileName);
		virtual ~OSMBlobFile();

		bool open();
		void close();
		void reset();

		BlobDataType readBlobData(char * & buffer, uint32_t & bufferSize);
		bool writeBlobData(BlobDataType type, char * buffer, uint32_t bufferSize);
	private:
		std::string m_FileName;

		int m_FileDescriptor;
		char * m_FileData;
		uint32_t m_FilePos;
		uint32_t m_FileSize;

		inline void * fileData() { return (void *) &(m_FileData[m_FilePos]); }
	};
}

#endif // OSMPBF_OSMBLOBFILE_H
