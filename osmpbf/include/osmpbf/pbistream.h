#ifndef OSMPBF_PBISTREAM_H
#define OSMPBF_PBISTREAM_H
#include <osmpbf/typelimits.h>
#include <osmpbf/osmfilein.h>
#include <memory>
#include <vector>

namespace osmpbf {
namespace interface {

///for thread-safety gua
class PbiStream {
public:
	PbiStream();
	virtual ~PbiStream() = 0;

	virtual void reset() = 0;
	virtual void seek(OffsetType position) = 0;
	virtual OffsetType position() const = 0;
	virtual OffsetType size() const = 0;

	virtual bool hasNext() const = 0;
	virtual bool getNext(BlobDataBuffer & buffer) = 0;
	virtual bool getNext(BlobDataMultiBuffer & buffers, int num) = 0;
	virtual bool parseNext(PrimitiveBlockInputAdaptor & adaptor) = 0;

};

}//end namespace interface

namespace imp {

class SingleFilePbiStream: public interface::PbiStream {
public:
	SingleFilePbiStream(OSMFileIn && file);
	virtual ~SingleFilePbiStream() override;

	virtual void reset() override;
	virtual void seek(OffsetType position) override;
	virtual OffsetType position() const override;
	virtual OffsetType size() const override;

	virtual bool hasNext() const override;
	virtual bool getNext(BlobDataBuffer & buffer) override;
	virtual bool getNext(BlobDataMultiBuffer & buffers, int num) override;
	virtual bool parseNext(PrimitiveBlockInputAdaptor & adaptor) override;
private:
	OSMFileIn m_file;
};

class MultiFilePbiStream: public interface::PbiStream {
public:
	///distance(begin, end) > 0!
	template<typename T_OSMFILE_IN_ITERATOR>
	MultiFilePbiStream(T_OSMFILE_IN_ITERATOR begin, T_OSMFILE_IN_ITERATOR end);
	virtual ~MultiFilePbiStream() override;

	virtual void reset() override;
	virtual void seek(OffsetType position) override;
	virtual OffsetType position() const override;
	virtual OffsetType size() const override;

	virtual bool hasNext() const override;
	virtual bool getNext(BlobDataBuffer & buffer) override;
	virtual bool getNext(BlobDataMultiBuffer & buffers, int num) override;
	virtual bool parseNext(PrimitiveBlockInputAdaptor & adaptor) override;
protected:
	OSMFileIn & currentFile();
	const OSMFileIn & currentFile() const;
private:
	std::vector<OSMFileIn> m_files;
	std::vector<OffsetType> m_clDataSize; //cumulative data size
	OffsetType m_dataSize;
	OffsetType m_position;
	std::size_t m_currentFile;
};

}//end namespace imp


class PbiStream {
public:
	PbiStream();
	PbiStream(OSMFileIn && fileIn);
	PbiStream(std::vector<OSMFileIn> && files);
	PbiStream(const std::vector<std::string> & fileNames);
	///@param begin iterator to OsmFileIn
	///@warning this moves the data between begin and end into PbiStream!
	template<typename T_OSMFILE_IN_ITERATOR>
	PbiStream(T_OSMFILE_IN_ITERATOR begin, T_OSMFILE_IN_ITERATOR end);
	virtual ~PbiStream();
	
	void reset();
	void seek(OffsetType position);
	
	OffsetType position() const;

	OffsetType size() const;

	bool hasNext() const;

	/**
	 * copy next block into data buffer
	 * not thread safe
	 *
	 * @param buffer target buffer
	 */
	bool getNext(BlobDataBuffer & buffer);

	/**
	 * copy next blocks into data buffers
	 * not thread-safe
	 *
	 * @param buffers target container of buffers
	 * @param num number of blocks to copy, set to -1 to copy all remaining blocks
	 */
	bool getNext(BlobDataMultiBuffer & buffers, int num);

	///@param adaptor parse next block by @adaptor, not thread-safe
	bool parseNext(PrimitiveBlockInputAdaptor & adaptor);
	
	
	// the following are the same as above,
	// but have the same name as the functions in OsmFileIn for easier migration
	void dataSeek(OffsetType position);
	OffsetType dataPosition() const;
	OffsetType dataSize() const;
	bool getNextBlock(BlobDataBuffer & buffer);

	bool getNextBlocks(BlobDataMultiBuffer & buffers, int num);
	bool parseNextBlock(PrimitiveBlockInputAdaptor & adaptor);
private:
	std::unique_ptr<interface::PbiStream> m_priv;
};

}//end namespace osmpbf

//now the definitions of template functions
namespace osmpbf {
namespace imp {

template<typename T_OSMFILE_IN_ITERATOR>
MultiFilePbiStream::MultiFilePbiStream(T_OSMFILE_IN_ITERATOR begin, T_OSMFILE_IN_ITERATOR end) :
m_dataSize(0),
m_position(0),
m_currentFile(0)
{
	using std::distance;
	auto dst = distance(begin, end);
	m_files.reserve(dst);
	m_clDataSize.reserve(dst+1);
	for(; begin != end; ++begin) {
		m_clDataSize.emplace_back(m_dataSize);
		m_dataSize += begin->dataSize();
		m_files.emplace_back( std::move(*begin) );
	}
	m_clDataSize.emplace_back(m_dataSize);
}


}//end namespace imp

template<typename T_OSMFILE_IN_ITERATOR>
PbiStream::PbiStream(T_OSMFILE_IN_ITERATOR begin, T_OSMFILE_IN_ITERATOR end)
{
	using std::distance;
	if (distance(begin, end) > 0) {
		m_priv.reset(new imp::MultiFilePbiStream(begin, end));
	}
}

}

#endif