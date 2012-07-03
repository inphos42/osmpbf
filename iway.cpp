#include "iway.h"

#include "primitiveblockinputadaptor.h"
#include "osmformat.pb.h"

namespace osmpbf {

// IWay

	IWay::IWay() : RCWrapper< WayInputAdaptor >() {};
	IWay::IWay(WayInputAdaptor * data) : RCWrapper< WayInputAdaptor >(data) {}
	IWay::IWay(const IWay & other) : RCWrapper< WayInputAdaptor >(other) {}

// IWayStream

	IWayStream::IWayStream(PrimitiveBlockInputAdaptor * controller) : IWay(new WayStreamInputAdaptor(controller)) {}
	IWayStream::IWayStream(const IWayStream & other) : IWay(other) {}

	IWayStream & IWayStream::operator=(IWayStream & other) { IWay::operator=(other); return *this; }

// WayInputAdaptor

	WayInputAdaptor::WayInputAdaptor() : AbstractPrimitiveInputAdaptor() {}
	WayInputAdaptor::WayInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Way * data)
		: AbstractPrimitiveInputAdaptor(controller), m_Data(data) {}

	int64_t WayInputAdaptor::id() {
		return m_Data->id();
	}

	int64_t WayInputAdaptor::ref(int index) const {
		int64_t result = m_Data->refs(0);
		for (int i = 0; i < index; i++)
			result += m_Data->refs(i);

		return result;
	}

	int WayInputAdaptor::refsSize() const {
		return m_Data->refs_size();
	}

	int64_t WayInputAdaptor::rawRef(int index) const {
		return m_Data->refs(index);
	}

	DeltaFieldConstForwardIterator<int64_t> WayInputAdaptor::refBegin() const {
		return DeltaFieldConstForwardIterator<int64_t>(m_Data->refs().data());
	}

	DeltaFieldConstForwardIterator<int64_t> WayInputAdaptor::refEnd() const {
		return DeltaFieldConstForwardIterator<int64_t>(m_Data->refs().data() + m_Data->refs_size());
	}

	int WayInputAdaptor::tagsSize() const {
		return m_Data->keys_size();
	}

	uint32_t WayInputAdaptor::keyId(int index) const {
		return m_Data->keys(index);
	}

	uint32_t WayInputAdaptor::valueId(int index) const {
		return m_Data->vals(index);
	}

// WayStreamInputAdaptor

	WayStreamInputAdaptor::WayStreamInputAdaptor() : WayInputAdaptor(), m_Index(0), m_MaxIndex(0) {}
	WayStreamInputAdaptor::WayStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller)
		: WayInputAdaptor(controller, controller->m_WaysGroup ? controller->m_WaysGroup->ways().data()[0] : NULL), m_Index(0), m_MaxIndex(m_Controller->waysSize()) {}

	bool WayStreamInputAdaptor::isNull() const {
		return AbstractPrimitiveInputAdaptor::isNull() || (m_Index >= m_MaxIndex);
	}

	void WayStreamInputAdaptor::next() {
		m_Index++;
		m_Data = m_Controller->m_WaysGroup->ways().data()[m_Index];
	}

	void WayStreamInputAdaptor::previous() {
		m_Index--;
		m_Data = m_Controller->m_WaysGroup->ways().data()[m_Index];
	}
}
