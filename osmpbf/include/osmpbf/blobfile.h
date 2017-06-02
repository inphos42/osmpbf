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

#include <mutex>

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
	virtual SizeType position() const = 0;

	virtual SizeType size() const = 0;

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

	///Only makes sense in single-thread usage
	virtual void seek(OffsetType position) override;
	///Only makes sense in single-thread usage
	virtual SizeType position() const override;

	virtual SizeType size() const override;
	
	///thread-safe
	void readBlob(BlobDataBuffer & buffer);
	///thread-safe
	BlobDataType readBlob(char * & buffer, uint32_t & bufferSize, uint32_t & availableDataSize);

	///Only makes sense in single-thread usage
	bool skipBlob();

protected:
	char * m_FileData;
	std::mutex m_fileLock;
	SizeType m_FilePos;
	SizeType m_FileSize;

	void readBlobHeader(uint32_t & blobLength, BlobDataType & blobDataType);

	void * fileData();
	void * fileData(SizeType _position);

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
	virtual SizeType position() const override;

	virtual SizeType size() const override;

	bool writeBlob(const BlobDataBuffer & buffer, bool compress = true);
	bool writeBlob(BlobDataType type, const char * buffer, uint32_t bufferSize, bool compress = true);

protected:
	SizeType m_CurrentSize;

private:
	BlobFileOut() = delete;
};

} // namespace osmpbf

#endif // OSMPBF_BLOBFILE_H
