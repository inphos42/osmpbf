#include "osmnode.h"

#include "osmdatacontroller.h"
#include "osmformat.pb.h"

namespace osmpbf {

// OSMNode

	OSMNode::OSMNode() : m_Private(NULL) {}
	OSMNode::OSMNode(const OSMNode & other) : m_Private(other.m_Private) {
		if (m_Private) m_Private->refInc();
	}

	OSMNode::OSMNode(AbstractOSMNodeAdaptor * data) : m_Private(data) {
		if (m_Private) m_Private->refInc();
	}

	OSMNode::~OSMNode() {
		if (m_Private) m_Private->refDec();
	}

	OSMNode & OSMNode::operator=(const OSMNode & other) {
		if (m_Private)
			m_Private->refDec();
		m_Private = other.m_Private;
		if (m_Private)
			m_Private->refInc();
		return *this;
	}

// OSMStreamNode

	OSMStreamNode::OSMStreamNode(OSMPrimitiveBlockController * controller) : OSMNode(new OSMStreamNodeAdaptor(controller)) {}
	OSMStreamNode::OSMStreamNode(const OSMStreamNode & other): OSMNode(other) {}

// OSMStreamNodeAdaptor

	OSMStreamNodeAdaptor::OSMStreamNodeAdaptor(OSMPrimitiveBlockController * controller) :
		AbstractOSMNodeAdaptor(controller, controller->m_NodesGroup, 0),
		m_DenseGroup(controller->m_DenseNodesGroup),
		m_NodesSize(m_Group ? m_Group->nodes_size() : 0),
		m_DenseNodesSize(m_DenseGroup ? m_DenseGroup->dense().id_size() : 0),
		m_Id(0),
		m_Lat(0), m_Lon(0),
		m_WGS84Lat(0), m_WGS84Lon(0)
	{}

	void OSMStreamNodeAdaptor::next() {
		if (isNull())
			return;

		m_Index++;
		m_DenseIndex = m_Index - m_NodesSize;

		if (m_DenseIndex < 0) {
			m_Id = m_Group->nodes(m_Index).id();
			m_Lat = m_Group->nodes(m_Index).lat();
			m_Lon = m_Group->nodes(m_Index).lon();
		}
		else if (m_DenseIndex > 0) {
			if (m_Controller->denseNodesUnpacked()) {
				m_Id = m_DenseGroup->dense().id(m_DenseIndex);
				m_Lat = m_DenseGroup->dense().lat(m_DenseIndex);
				m_Lon = m_DenseGroup->dense().lon(m_DenseIndex);
			}
			else {
				m_Id += m_DenseGroup->dense().id(m_DenseIndex);
				m_Lat += m_DenseGroup->dense().lat(m_DenseIndex);
				m_Lon += m_DenseGroup->dense().lon(m_DenseIndex);
			}
		}
		else {
			m_Id = m_DenseGroup->dense().id(0);
			m_Lat = m_DenseGroup->dense().lat(0);
			m_Lon = m_DenseGroup->dense().lon(0);
		}

		m_WGS84Lat = m_Controller->toWGS84Lat(m_Lat);
		m_WGS84Lon = m_Controller->toWGS84Lon(m_Lon);
	}

	void OSMStreamNodeAdaptor::previous() {
		if (isNull())
			return;

		m_Index--;

		m_DenseIndex = m_Index - m_NodesSize;
		if (m_DenseIndex < 0) {
			m_Id = m_Group->nodes(m_Index).id();
			m_Lat = m_Group->nodes(m_Index).lat();
			m_Lon = m_Group->nodes(m_Index).lon();
		}
		else if (m_DenseIndex > 0) {
			if (m_Controller->denseNodesUnpacked()) {
				m_Id = m_DenseGroup->dense().id(m_DenseIndex);
				m_Lat = m_DenseGroup->dense().lat(m_DenseIndex);
				m_Lon = m_DenseGroup->dense().lon(m_DenseIndex);
			}
			else {
				m_Id -= m_DenseGroup->dense().id(m_DenseIndex + 1);
				m_Lat -= m_DenseGroup->dense().lat(m_DenseIndex + 1);
				m_Lon -= m_DenseGroup->dense().lon(m_DenseIndex + 1);
			}
		}
		else {
			m_Id = m_DenseGroup->dense().id(0);
			m_Lat = m_DenseGroup->dense().lat(0);
			m_Lon = m_DenseGroup->dense().lon(0);
		}

		m_WGS84Lat = m_Controller->toWGS84Lat(m_Lat);
		m_WGS84Lon = m_Controller->toWGS84Lon(m_Lon);
	}

	int OSMStreamNodeAdaptor::keysSize() const {
		return (m_DenseIndex < 0) ?
			m_Group->nodes(m_Index).keys_size() :
			m_Controller->queryDenseNodeKeyValIndex(m_DenseIndex * 2 + 1);
	}

	int OSMStreamNodeAdaptor::keyId(int index) const {
		if (index < 0 || index > keysSize())
			return -1;

		return (m_DenseIndex < 0) ?
			m_Group->nodes(m_Index).keys(index) :
			m_DenseGroup->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_DenseIndex * 2) + index * 2);
	}

	int OSMStreamNodeAdaptor::valueId(int index) const {
		if (index < 0 || index > keysSize())
			return -1;

		return (m_DenseIndex < 0) ?
			m_Group->nodes(m_Index).vals(index) :
			m_DenseGroup->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_DenseIndex * 2) + index * 2 + 1);
	}

	std::string OSMStreamNodeAdaptor::key(int index) const {
		int id = keyId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}

	std::string OSMStreamNodeAdaptor::value(int index) const {
		int id = valueId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}

// OSMPlainNodeAdaptor

	OSMPlainNodeAdaptor::OSMPlainNodeAdaptor() : AbstractOSMNodeAdaptor() {}
	OSMPlainNodeAdaptor::OSMPlainNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position) :
		AbstractOSMNodeAdaptor(controller, group, position) {}

	int64_t OSMPlainNodeAdaptor::id() {
		return m_Group->nodes(m_Index).id();
	}

	int64_t OSMPlainNodeAdaptor::lat() {
		return m_Controller->toWGS84Lat(m_Group->nodes(m_Index).lat());
	}

	int64_t OSMPlainNodeAdaptor::lon() {
		return m_Controller->toWGS84Lon(m_Group->nodes(m_Index).lon());
	}

	int64_t OSMPlainNodeAdaptor::rawLat() const {
		return m_Group->nodes(m_Index).lat();
	}

	int64_t OSMPlainNodeAdaptor::rawLon() const {
		return m_Group->nodes(m_Index).lon();
	}

	int OSMPlainNodeAdaptor::keysSize() const {
		return m_Group->nodes(m_Index).keys_size();
	}

	int OSMPlainNodeAdaptor::keyId(int index) const {
		return m_Group->nodes(m_Index).keys(index);
	}

	int OSMPlainNodeAdaptor::valueId(int index) const {
		return m_Group->nodes(m_Index).vals(index);
	}
	std::string OSMPlainNodeAdaptor::key(int index) const {
		return m_Controller->queryStringTable(m_Group->nodes(m_Index).keys(index));
	}

	std::string OSMPlainNodeAdaptor::value(int index) const {
		return m_Controller->queryStringTable(m_Group->nodes(m_Index).vals(index));
	}

// OSMDenseNodeAdaptor

	OSMDenseNodeAdaptor::OSMDenseNodeAdaptor() : AbstractOSMNodeAdaptor() {}
	OSMDenseNodeAdaptor::OSMDenseNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position) :
		AbstractOSMNodeAdaptor(controller, group, position) {}

	int64_t OSMDenseNodeAdaptor::id() {
		if (!m_HasCachedId) {
			m_CachedId = m_Group->dense().id(0);
			for (int i = 0; i < m_Index; i++)
				m_CachedId += m_Group->dense().id(i);
		}

		return m_CachedId;
	}

	int64_t OSMDenseNodeAdaptor::lat() {
		if (m_Controller->denseNodesUnpacked())
			m_Controller->toWGS84Lat(m_Group->dense().lat(m_Index));

		if (!m_HasCachedLat) {
			m_CachedLat = m_Group->dense().lat(0);
			for (int i = 0; i < m_Index; i++)
				m_CachedLat += m_Group->dense().lat(i);
		}

		return m_Controller->toWGS84Lat(m_CachedLat);
	}

	int64_t OSMDenseNodeAdaptor::lon() {
		if (!m_HasCachedLon) {
			m_CachedLon = m_Group->dense().lon(0);
			for (int i = 0; i < m_Index; i++)
				m_CachedLon += m_Group->dense().lon(i);
		}

		return m_Controller->toWGS84Lon(m_CachedLon);
	}

	int64_t OSMDenseNodeAdaptor::rawLat() const {
		return m_Group->dense().lat(m_Index);
	}

	int64_t OSMDenseNodeAdaptor::rawLon() const {
		return m_Group->dense().lon(m_Index);
	}

	int OSMDenseNodeAdaptor::keysSize() const {
		return (!m_Group->dense().keys_vals_size()) ? 0 : m_Controller->queryDenseNodeKeyValIndex(m_Index * 2 + 1);
	}

	int OSMDenseNodeAdaptor::keyId(int index) const {
		return (index < 0 || index >= keysSize()) ? -1 : m_Group->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_Index * 2) + index * 2);
	}

	int OSMDenseNodeAdaptor::valueId(int index) const {
		return (index < 0 || index >= keysSize()) ? -1 : m_Group->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_Index * 2) + index * 2 + 1);
	}

	std::string OSMDenseNodeAdaptor::key(int index) const {
		int id = keyId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}

	std::string OSMDenseNodeAdaptor::value(int index) const {
		int id = valueId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}
}
