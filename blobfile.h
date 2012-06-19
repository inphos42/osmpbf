#ifndef OSMPBF_BLOBFILE_H
#define OSMPBF_BLOBFILE_H

#include <cstdint>
#include <string>

#include "blobdata.h"

namespace osmpbf {
	class AbstractBlobFile {
	public:
		AbstractBlobFile() : m_FileDescriptor(-1), m_VerboseOutput(false) {}
		AbstractBlobFile(const std::string & fileName);
		virtual ~AbstractBlobFile() {}

		virtual bool open() = 0;
		virtual void close() = 0;

		virtual void seek(uint32_t position) = 0;
		virtual uint32_t position() const = 0;

		virtual uint32_t size() const = 0;

		inline void setVerboseOutput(bool value) { m_VerboseOutput = value; }
	protected:
		std::string m_FileName;
		int m_FileDescriptor;
		bool m_VerboseOutput;
	};

	class BlobFileIn : public AbstractBlobFile {
	public:
		BlobFileIn(const std::string & fileName) : AbstractBlobFile(fileName), m_FileData(NULL) {}
		virtual ~BlobFileIn() { close(); }

		virtual bool open();
		virtual void close();

		virtual void seek(uint32_t position) { m_FilePos = position; }
		virtual uint32_t position() const { return m_FilePos; }

		virtual uint32_t size() const { return m_FileSize; }

		void readBlob(BlobDataBuffer & buffer);
		BlobDataType readBlob(char * & buffer, uint32_t & bufferSize, uint32_t & availableDataSize);

		bool skipBlob();

	protected:
		char * m_FileData;
		uint32_t m_FilePos;
		uint32_t m_FileSize;

		void readBlobHeader(uint32_t & blobLength, BlobDataType & blobDataType);

		inline void * fileData() { return (void *) &(m_FileData[m_FilePos]); }

	private:
		BlobFileIn() : AbstractBlobFile() {}
	};

	class BlobFileOut : public AbstractBlobFile {
	public:
		BlobFileOut(const std::string & fileName) : AbstractBlobFile(fileName), m_CurrentSize(0) {}
		virtual ~BlobFileOut() { close(); }

		virtual bool open();
		virtual void close();

		virtual void seek(uint32_t position);
		virtual uint32_t position() const;

		virtual uint32_t size() const { return m_CurrentSize; }

		bool writeBlob(const BlobDataBuffer & buffer, bool compress = true);
		bool writeBlob(BlobDataType type, const char * buffer, uint32_t bufferSize, bool compress = true);

	protected:
		uint32_t m_CurrentSize;

	private:
		BlobFileOut() : AbstractBlobFile() {}
	};
}

#endif // OSMPBF_BLOBFILE_H
