#ifndef OSMPBF_BLOBFILE_H
#define OSMPBF_BLOBFILE_H

#include <cstdint>
#include <string>

namespace osmpbf {
	enum BlobDataType {BLOB_Invalid = 0, BLOB_OSMHeader = 1, BLOB_OSMData = 2};

	class AbstractBlobFile {
	public:
		AbstractBlobFile() : m_FileDescriptor(-1), m_VerboseOutput(false) {}
		AbstractBlobFile(std::string fileName);
		virtual ~AbstractBlobFile() {}

		virtual bool open() = 0;
		virtual void close() = 0;

		virtual void seek(uint32_t position) = 0;
		virtual uint32_t position() const = 0;

		inline void setVerboseOutput(bool value) { m_VerboseOutput = value; }
	protected:
		std::string m_FileName;
		int m_FileDescriptor;
		bool m_VerboseOutput;
	};

	class BlobFileIn : public AbstractBlobFile {
	public:
		BlobFileIn(std::string fileName) : AbstractBlobFile(fileName), m_FileData(NULL) {}
		virtual ~BlobFileIn() { close(); }

		virtual bool open();
		virtual void close();

		virtual void seek(uint32_t position) { m_FilePos = position; }
		virtual uint32_t position() const { return m_FilePos; }

		BlobDataType readBlob(char * & buffer, uint32_t & bufferSize);
	private:
		char * m_FileData;
		uint32_t m_FilePos;
		uint32_t m_FileSize;

		inline void * fileData() { return (void *) &(m_FileData[m_FilePos]); }
	};

	class BlobFileOut : public AbstractBlobFile {
	public:
		BlobFileOut(std::string fileName) : AbstractBlobFile(fileName) {}
		virtual ~BlobFileOut() { close(); }

		virtual bool open();
		virtual void close();

		virtual void seek(uint32_t position);
		virtual uint32_t position() const;

		bool writeBlob(BlobDataType type, char * buffer, uint32_t bufferSize, bool compress = true);
	};
}

#endif // OSMPBF_BLOBFILE_H
