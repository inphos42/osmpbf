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

		virtual uint32_t size() const = 0;

		inline void setVerboseOutput(bool value) { m_VerboseOutput = value; }
	protected:
		std::string m_FileName;
		int m_FileDescriptor;
		bool m_VerboseOutput;
	};

	struct BlobDataBuffer {
		BlobDataType type;
		char * data;
		uint32_t availableBytes;
		uint32_t totalBytes;

		inline void clear() {
			if (data)
				delete[] data;
			data = NULL;
			availableBytes = 0;
			totalBytes = 0;
		}

		BlobDataBuffer() : type(BLOB_Invalid), data(NULL), availableBytes(0), totalBytes(0) {}
		~BlobDataBuffer() { clear(); }
	};

	class BlobFileIn : public AbstractBlobFile {
	public:
		BlobFileIn(std::string fileName) : AbstractBlobFile(fileName), m_FileData(NULL) {}
		virtual ~BlobFileIn() { close(); }

		virtual bool open();
		virtual void close();

		virtual void seek(uint32_t position) { m_FilePos = position; }
		virtual uint32_t position() const { return m_FilePos; }

		virtual uint32_t size() const { return m_FileSize; }

		void readBlob(BlobDataBuffer & buffer);
		BlobDataType readBlob(char * & buffer, uint32_t & bufferSize, uint32_t & availableDataSize);
	private:
		char * m_FileData;
		uint32_t m_FilePos;
		uint32_t m_FileSize;

		BlobFileIn() : AbstractBlobFile() {}

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

		virtual uint32_t size() const;

		bool writeBlob(const BlobDataBuffer & buffer, bool compress = true);
		bool writeBlob(BlobDataType type, const char * buffer, uint32_t bufferSize, bool compress = true);
	private:
		BlobFileOut() : AbstractBlobFile() {}
	};
}

#endif // OSMPBF_BLOBFILE_H
