#include "osmway.h"

#include "osmdatacontroller.h"
#include "osmformat.pb.h"

namespace osmpbf {

// OSMStreamWay

	OSMStreamWay::OSMStreamWay(OSMPrimitiveBlockInputAdaptor * controller) : OSMWay(new OSMStreamWayAdaptor(controller)) {}
	OSMStreamWay::OSMStreamWay(const osmpbf::OSMStreamWay & other) : OSMWay(other) {}

// OSMWayAdaptor

	OSMWayAdaptor::OSMWayAdaptor() : AbstractOSMPrimitiveAdaptor() {}
	OSMWayAdaptor::OSMWayAdaptor(OSMPrimitiveBlockInputAdaptor * controller, PrimitiveGroup * group, int position)
		: AbstractOSMPrimitiveAdaptor(controller, group, position) {}

	int64_t OSMWayAdaptor::id() {
		return m_Group->ways(m_Index).id();
	}

	int64_t OSMWayAdaptor::ref(int index) const {
		int64_t result = m_Group->ways(m_Index).refs(0);
		for (int i = 0; i < index; i++)
			result += m_Group->ways(m_Index).refs(i);

		return result;
	}

	int OSMWayAdaptor::refsSize() const {
		return m_Group->ways(m_Index).refs_size();
	}

	int64_t OSMWayAdaptor::rawRef(int index) const {
		return m_Group->ways(m_Index).refs(index);
	}

	DeltaFieldConstForwardIterator<int64_t> OSMWayAdaptor::refBegin() const {
		return m_Group->ways(m_Index).refs().data();
	}

	DeltaFieldConstForwardIterator<int64_t> OSMWayAdaptor::refEnd() const {
		return m_Group->ways(m_Index).refs().data() + m_Group->ways(m_Index).refs_size();
	}

	int OSMWayAdaptor::keysSize() const {
		return m_Group->ways(m_Index).keys_size();
	}

	int OSMWayAdaptor::keyId(int index) const {
		return m_Group->ways(m_Index).keys(index);
	}

	int OSMWayAdaptor::valueId(int index) const {
		return m_Group->ways(m_Index).vals(index);
	}
	std::string OSMWayAdaptor::key(int index) const {
		return m_Controller->queryStringTable(m_Group->ways(m_Index).keys(index));
	}

	std::string OSMWayAdaptor::value(int index) const {
		return m_Controller->queryStringTable(m_Group->ways(m_Index).vals(index));
	}

// OSMStreamWayAdaptor

	OSMStreamWayAdaptor::OSMStreamWayAdaptor() {}
	OSMStreamWayAdaptor::OSMStreamWayAdaptor(OSMPrimitiveBlockInputAdaptor * controller) :
		OSMWayAdaptor(controller, controller->m_WaysGroup, 0),
		m_WaysSize(m_Controller->waysSize())
	{}

	bool OSMStreamWayAdaptor::isNull() const {
		return AbstractOSMPrimitiveAdaptor::isNull() || (m_Index > m_WaysSize - 1);
	}

	void OSMStreamWayAdaptor::next() {
		if (m_Index < m_WaysSize) m_Index++;
	}

	void OSMStreamWayAdaptor::previous() {
		if (m_Index > -1) m_Index--;
	}
}
