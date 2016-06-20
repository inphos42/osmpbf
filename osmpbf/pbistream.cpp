#include <osmpbf/pbistream.h>
#include <osmpbf/osmfilein.h>
#include <assert.h>
#include <limits>

namespace osmpbf {
namespace interface {

PbiStream::PbiStream() {}

PbiStream::~PbiStream() {}

} //end namespace interface

namespace imp {

SingleFilePbiStream::SingleFilePbiStream(OSMFileIn&& file) :
m_file(std::move(file))
{}

SingleFilePbiStream::~SingleFilePbiStream() {}

void
SingleFilePbiStream::reset() {
	m_file.reset();
}

void
SingleFilePbiStream::seek(OffsetType position) {
	m_file.dataSeek(position);
}

OffsetType
SingleFilePbiStream::position() const {
	return m_file.dataPosition();
}

OffsetType
SingleFilePbiStream::size() const {
	return m_file.dataPosition();
}

bool
SingleFilePbiStream::hasNext() const {
	return m_file.hasNext();
}

bool
SingleFilePbiStream::getNext(BlobDataBuffer& buffer) {
	return m_file.getNextBlock(buffer);
}

bool
SingleFilePbiStream::getNext(osmpbf::BlobDataMultiBuffer& buffers, int num) {
	return m_file.getNextBlocks(buffers, num);
}

bool
SingleFilePbiStream::parseNext(PrimitiveBlockInputAdaptor& adaptor) {
	return m_file.parseNextBlock(adaptor);
}

MultiFilePbiStream::~MultiFilePbiStream() {}

const OSMFileIn&
MultiFilePbiStream::currentFile() const {
	return m_files[m_currentFile];
}

OSMFileIn&
MultiFilePbiStream::currentFile() {
	return m_files[m_currentFile];
}


void
MultiFilePbiStream::reset() {
	for(std::size_t i(0), s(std::min(m_currentFile+1, m_files.size())); i < s; ++i) {
		m_files[i].reset();
	}
	m_position = 0;
	m_currentFile = 0;
}

void
MultiFilePbiStream::seek(OffsetType position) {
	if (position > m_dataSize) {
		m_position = m_dataSize;
		m_currentFile = m_files.size();
		return;
	}
	if (position > m_position) {
		m_position = m_clDataSize[m_currentFile];
	}
	else {
		m_position = 0;
		m_currentFile = 0;
	}
	while(m_currentFile < m_files.size()) {
		m_position = m_clDataSize[m_currentFile];
		if (m_position + currentFile().dataSize() < position) {
			m_currentFile += 1;
		}
		else {
			position -= m_position;
			currentFile().dataSeek(position);
			m_position += position;
			break;
		}
	}
	assert(m_position < m_dataSize);
	assert(m_currentFile < m_files.size());
}

OffsetType
MultiFilePbiStream::position() const {
	return m_position;
}

OffsetType
MultiFilePbiStream::size() const {
	return m_dataSize;
}

bool
MultiFilePbiStream::hasNext() const {
	return m_currentFile < m_files.size() && currentFile().hasNext();
}

bool
MultiFilePbiStream::getNext(BlobDataBuffer & buffer) {
	if (!hasNext()) {
		return false;
	}
	bool ok = currentFile().getNextBlock(buffer);
	if (!currentFile().hasNext()) {
		m_currentFile += 1;
		m_position = m_clDataSize.at(m_currentFile);
		if (m_currentFile < m_files.size()) {
			currentFile().reset();
		}
	}
	return ok;
}

bool
MultiFilePbiStream::getNext(osmpbf::BlobDataMultiBuffer & buffers, int num) {
	osmpbf::BlobDataMultiBuffer tmpBuffer;
	if (num < 0) {
		num = std::numeric_limits<int>::max();
	}
	while(hasNext() && buffers.size() < (std::size_t)num) {
		int remNum = num-(int)buffers.size();
		
		currentFile().getNextBlocks(tmpBuffer, remNum);
		
		buffers.reserve(buffers.size() + tmpBuffer.size());
		for(auto & x : tmpBuffer) {
			buffers.emplace_back(std::move(x));
		}
		tmpBuffer.clear();
		
		if (currentFile().hasNext()) {
			m_position = m_clDataSize.at(m_currentFile) + currentFile().dataPosition();
		}
		else {
			m_currentFile += 1;
			m_position = m_clDataSize.at(m_currentFile);
			if (m_currentFile < m_files.size()) {
				currentFile().reset();
			}
		}
	}
	return (buffers.size() == (std::size_t)num) || (num == std::numeric_limits<int>::max());
}

bool
MultiFilePbiStream::parseNext(PrimitiveBlockInputAdaptor& adaptor) {
	if (!hasNext()) {
		return false;
	}
	bool ok = currentFile().parseNextBlock(adaptor);
	if (!currentFile().hasNext()) {
		m_currentFile += 1;
		m_position = m_clDataSize.at(m_currentFile);
		if (m_currentFile < m_files.size()) {
			currentFile().reset();
		}
	}
	return ok;
}


}//end namespace imp


PbiStream::PbiStream() {}

PbiStream::PbiStream(OSMFileIn && fileIn) :
m_priv(new imp::SingleFilePbiStream(std::move(fileIn)))
{}

PbiStream::PbiStream(std::vector<OSMFileIn> && files) {
	if (files.size()) {
		m_priv.reset(new imp::MultiFilePbiStream(files.begin(), files.end()));
		files.clear();
	}
}

PbiStream::PbiStream(const std::vector<std::string> & fileNames) {
	std::vector<OSMFileIn> files;
	files.reserve(fileNames.size());
	for(const std::string & fileName : fileNames) {
		OSMFileIn tmpFile(fileName);
		if (tmpFile.open()) {
			files.emplace_back(std::move(tmpFile));
		}
		else {
			throw std::runtime_error("osmpbf::PbiStream: could not open file " + fileName);
		}
	}
	if (files.size() == 1) {
		m_priv.reset(new imp::SingleFilePbiStream(std::move(files[0])));
	}
	else if (files.size() > 1) {
		m_priv.reset(new imp::MultiFilePbiStream(files.begin(), files.end()));
	}
}


PbiStream::~PbiStream() {}

void
PbiStream::reset() {
	m_priv->reset();
}

void
PbiStream::seek(OffsetType position) {
	m_priv->seek(position);
}

OffsetType
PbiStream::position() const {
	return m_priv->position();
}

OffsetType
PbiStream::size() const {
	return m_priv->size();
}

bool
PbiStream::hasNext() const {
	return m_priv->hasNext();
}

bool
PbiStream::getNext(BlobDataBuffer & buffer) {
	return m_priv->getNext(buffer);
}

bool
PbiStream::getNext(BlobDataMultiBuffer & buffers, int num) {
	return m_priv->getNext(buffers, num);
}

bool
PbiStream::parseNext(PrimitiveBlockInputAdaptor & adaptor) {
	return m_priv->parseNext(adaptor);
}

//convinience functions for easier porting

void
PbiStream::dataSeek(OffsetType position) {
	return seek(position);
}

OffsetType
PbiStream::dataPosition() const {
	return position();
}

OffsetType
PbiStream::dataSize() const {
	return size();
}

bool
PbiStream::getNextBlock(BlobDataBuffer & buffer) {
	return getNext(buffer);
}

bool
PbiStream::getNextBlocks(BlobDataMultiBuffer & buffers, int num) {
	return getNext(buffers, num);
}

bool
PbiStream::parseNextBlock(PrimitiveBlockInputAdaptor & adaptor) {
	return parseNext(adaptor);
}

}//end namespace osmpbf