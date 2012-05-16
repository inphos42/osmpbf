#include "iway.h"

#include "primitiveblockinputadaptor.h"
#include "osmformat.pb.h"

namespace osmpbf {

// IWay

	IWay::IWay() : RCWrapper< WayInputAdaptor >() {};
	IWay::IWay(WayInputAdaptor * data) : RCWrapper< WayInputAdaptor >(data) {}
	IWay::IWay(const IWay & other) : RCWrapper< WayInputAdaptor >(other) {}

	IWay & IWay::operator=(const IWay & other) { RCWrapper::operator=(other); return *this; }

// IWayStream

	IWayStream::IWayStream(PrimitiveBlockInputAdaptor * controller) : IWay(new WayStreamInputAdaptor(controller)) {}
	IWayStream::IWayStream(const IWayStream & other) : IWay(other) {}

	IWayStream& IWayStream::operator=(IWayStream & other) { IWay::operator=(other); return *this; }

// WayInputAdaptor

	WayInputAdaptor::WayInputAdaptor() : AbstractPrimitiveInputAdaptor() {}
	WayInputAdaptor::WayInputAdaptor(PrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position)
		: AbstractPrimitiveInputAdaptor(controller, group, position) {}

	int64_t WayInputAdaptor::id() {
		return m_Group->ways(m_Index).id();
	}

	int64_t WayInputAdaptor::ref(int index) const {
		int64_t result = m_Group->ways(m_Index).refs(0);
		for (int i = 0; i < index; i++)
			result += m_Group->ways(m_Index).refs(i);

		return result;
	}

	int WayInputAdaptor::refsSize() const {
		return m_Group->ways(m_Index).refs_size();
	}

	int64_t WayInputAdaptor::rawRef(int index) const {
		return m_Group->ways(m_Index).refs(index);
	}

	DeltaFieldConstForwardIterator<int64_t> WayInputAdaptor::refBegin() const {
		return DeltaFieldConstForwardIterator<int64_t>(m_Group->ways(m_Index).refs().data());
	}

	DeltaFieldConstForwardIterator<int64_t> WayInputAdaptor::refEnd() const {
		return DeltaFieldConstForwardIterator<int64_t>(m_Group->ways(m_Index).refs().data() + m_Group->ways(m_Index).refs_size());
	}

	int WayInputAdaptor::tagsSize() const {
		return m_Group->ways(m_Index).keys_size();
	}

	int WayInputAdaptor::keyId(int index) const {
		return m_Group->ways(m_Index).keys(index);
	}

	int WayInputAdaptor::valueId(int index) const {
		return m_Group->ways(m_Index).vals(index);
	}

	const std::string & WayInputAdaptor::key(int index) const {
		return m_Controller->queryStringTable(m_Group->ways(m_Index).keys(index));
	}

	const std::string & WayInputAdaptor::value(int index) const {
		return m_Controller->queryStringTable(m_Group->ways(m_Index).vals(index));
	}

// WayStreamInputAdaptor

	WayStreamInputAdaptor::WayStreamInputAdaptor() {}
	WayStreamInputAdaptor::WayStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller) :
		WayInputAdaptor(controller, controller->m_WaysGroup, 0), m_WaysSize(m_Controller->waysSize()) {}

	bool WayStreamInputAdaptor::isNull() const {
		return AbstractPrimitiveInputAdaptor::isNull() || (m_Index > m_WaysSize - 1);
	}

	void WayStreamInputAdaptor::next() {
		m_Index++;
	}

	void WayStreamInputAdaptor::previous() {
		m_Index--;
	}
}
