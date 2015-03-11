/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2014 Oliver Groß.

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

#include <osmpbf/blobdata.h>
#include <osmpbf/typelimits.h>

#include <cstdint>
#include <string>

namespace osmpbf
{

class AbstractBlobFile
{
public:
	explicit AbstractBlobFile(const std::string & fileName);
	virtual ~AbstractBlobFile() {}

	virtual bool open() = 0;
	virtual void close() = 0;

	virtual void seek(OffsetType position) = 0;
	virtual OffsetType position() const = 0;

	virtual OffsetType size() const = 0;

	inline void setVerboseOutput(bool value)
	{
		m_VerboseOutput = value;
	}

protected:
	AbstractBlobFile() = delete;

	std::string m_FileName;
	int m_FileDescriptor;
	bool m_VerboseOutput;
};

class BlobFileIn : public AbstractBlobFile
{
public:
	explicit BlobFileIn(const std::string & fileName);
	virtual ~BlobFileIn();

	virtual bool open() override;
	virtual void close() override;

	virtual void seek(OffsetType position) override;
	virtual OffsetType position() const override;

	virtual OffsetType size() const override;

	void readBlob(BlobDataBuffer & buffer);
	BlobDataType readBlob(char * & buffer, uint32_t & bufferSize, uint32_t & availableDataSize);

	bool skipBlob();

protected:
	char * m_FileData;
	OffsetType m_FilePos;
	OffsetType m_FileSize;

	void readBlobHeader(uint32_t & blobLength, BlobDataType & blobDataType);

	inline void * fileData()
	{
		return static_cast<void *>(&(m_FileData[m_FilePos]));
	}

private:
	BlobFileIn() = delete;
};

class BlobFileOut : public AbstractBlobFile
{
public:
	explicit BlobFileOut(const std::string & fileName);
	virtual ~BlobFileOut();

	virtual bool open() override;
	virtual void close() override;

	virtual void seek(OffsetType position) override;
	virtual OffsetType position() const override;

	virtual OffsetType size() const override;

	bool writeBlob(const BlobDataBuffer & buffer, bool compress = true);
	bool writeBlob(BlobDataType type, const char * buffer, uint32_t bufferSize, bool compress = true);

protected:
	OffsetType m_CurrentSize;

private:
	BlobFileOut() = delete;
};

} // namespace osmpbf

#endif // OSMPBF_BLOBFILE_H
