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

		virtual void seek(OffsetType position) = 0;
		virtual OffsetType position() const = 0;

		virtual OffsetType size() const = 0;

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

		virtual void seek(OffsetType position) { m_FilePos = position; }
		virtual OffsetType position() const { return m_FilePos; }

		virtual OffsetType size() const { return m_FileSize; }

		void readBlob(BlobDataBuffer & buffer);
		BlobDataType readBlob(char * & buffer, OffsetType & bufferSize, OffsetType & availableDataSize);

		bool skipBlob();

	protected:
		char * m_FileData;
		OffsetType m_FilePos;
		OffsetType m_FileSize;

		void readBlobHeader(OffsetType & blobLength, BlobDataType & blobDataType);

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

		virtual void seek(OffsetType position);
		virtual OffsetType position() const;

		virtual OffsetType size() const { return m_CurrentSize; }

		bool writeBlob(const BlobDataBuffer & buffer, bool compress = true);
		bool writeBlob(BlobDataType type, const char * buffer, OffsetType bufferSize, bool compress = true);

	protected:
		OffsetType m_CurrentSize;

	private:
		BlobFileOut() : AbstractBlobFile() {}
	};
}

#endif // OSMPBF_BLOBFILE_H
