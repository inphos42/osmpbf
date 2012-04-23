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

	int64_t OSMWay::OSMWayAdaptor::ref(int index) {
		int64_t result = m_Group->ways(m_Position).refs(0);
		for (int i = 0; i < index; i++)
			result += m_Group->ways(m_Position).refs(i);

		return result;
	}

	int OSMWay::OSMWayAdaptor::refsSize() const {
		return m_Group->ways(m_Position).refs_size();
	}

	int64_t OSMWay::OSMWayAdaptor::rawRef(int index) {
		return m_Group->ways(m_Position).refs(index);
	}

	DeltaFieldConstForwardIterator<int64_t> OSMWay::OSMWayAdaptor::refBegin() const {
		return m_Group->ways(m_Position).refs().data();
	}

	DeltaFieldConstForwardIterator<int64_t> OSMWay::OSMWayAdaptor::refEnd() const {
		return m_Group->ways(m_Position).refs().data() + m_Group->ways(m_Position).refs_size();
	}

	int OSMWay::OSMWayAdaptor::keysSize() {
		return m_Group->ways(m_Position).keys_size();
	}

	std::string OSMWay::OSMWayAdaptor::key(int index) {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).keys(index));
	}

	std::string OSMWay::OSMWayAdaptor::value(int index) {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).vals(index));
	}

// 	// TODO
// 	std::string OSMWay::OSMWayAdaptor::value(std::string key) {
// 		return std::string();
// 	}
}
