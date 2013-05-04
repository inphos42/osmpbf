/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2013 Oliver Groß.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <http://www.gnu.org/licenses/>.
 */

#ifndef OSMPBF_BLOBFILE_H
#define OSMPBF_BLOBFILE_H

#include <cstdint>
#include <string>

#include "blobdata.h"

#include "typelimits.h"

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
		BlobDataType readBlob(char * & buffer, uint32_t & bufferSize, uint32_t & availableDataSize);

		bool skipBlob();

	protected:
		char * m_FileData;
		OffsetType m_FilePos;
		OffsetType m_FileSize;

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

		virtual void seek(OffsetType position);
		virtual OffsetType position() const;

		virtual OffsetType size() const { return m_CurrentSize; }

		bool writeBlob(const BlobDataBuffer & buffer, bool compress = true);
		bool writeBlob(BlobDataType type, const char * buffer, uint32_t bufferSize, bool compress = true);

	protected:
		OffsetType m_CurrentSize;

	private:
		BlobFileOut() : AbstractBlobFile() {}
	};
}

#endif // OSMPBF_BLOBFILE_H
