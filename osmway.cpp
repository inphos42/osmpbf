#include "osmway.h"

#include "osmdatacontroller.h"
#include "osmformat.pb.h"

namespace osmpbf {

// OSMWayAdaptor

	OSMWay::OSMWayAdaptor::OSMWayAdaptor() : AbstractOSMPrimitiveAdaptor() {}
	OSMWay::OSMWayAdaptor::OSMWayAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position)
		: AbstractOSMPrimitiveAdaptor(controller, group, position) {}

	int64_t OSMWay::OSMWayAdaptor::id() {
		return m_Group->ways(m_Position).id();
	}

	int64_t OSMWay::OSMWayAdaptor::ref(int index) const {
		int64_t result = m_Group->ways(m_Position).refs(0);
		for (int i = 0; i < index; i++)
			result += m_Group->ways(m_Position).refs(i);

		return result;
	}

	int OSMWay::OSMWayAdaptor::refsSize() const {
		return m_Group->ways(m_Position).refs_size();
	}

	int64_t OSMWay::OSMWayAdaptor::rawRef(int index) const {
		return m_Group->ways(m_Position).refs(index);
	}

	DeltaFieldConstForwardIterator<int64_t> OSMWay::OSMWayAdaptor::refBegin() const {
		return m_Group->ways(m_Position).refs().data();
	}

	DeltaFieldConstForwardIterator<int64_t> OSMWay::OSMWayAdaptor::refEnd() const {
		return m_Group->ways(m_Position).refs().data() + m_Group->ways(m_Position).refs_size();
	}

	int OSMWay::OSMWayAdaptor::keysSize() const {
		return m_Group->ways(m_Position).keys_size();
	}

	int OSMWay::OSMWayAdaptor::keyId(int index) const {
		return m_Group->ways(m_Position).keys(index);
	}

	int OSMWay::OSMWayAdaptor::valueId(int index) const {
		return m_Group->ways(m_Position).vals(index);
	}
	std::string OSMWay::OSMWayAdaptor::key(int index) const {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).keys(index));
	}

	std::string OSMWay::OSMWayAdaptor::value(int index) const {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).vals(index));
	}
}
