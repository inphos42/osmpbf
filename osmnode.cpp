#include "osmnode.h"

#include "osmdatacontroller.h"
#include "osmformat.pb.h"

namespace osmpbf {

// OSMNodeStream

	OSMNodeStream::OSMNodeStream(OSMPrimitiveBlockController * controller) :
		m_Controller(controller),
		m_Position(-1),
		m_NodesSize(controller->m_NodesGroup ? controller->m_NodesGroup->nodes_size() : 0),
		m_DenseNodesSize(controller->m_DenseNodesGroup ? controller->m_DenseNodesGroup->dense().id_size() : 0),
		m_Id(0),
		m_Lat(0),
		m_Lon(0)
	{}

	OSMNodeStream::OSMNodeStream(const OSMNodeStream & other) :
		m_Controller(other.m_Controller),
		m_Position(other.m_Position),
		m_NodesSize(other.m_NodesSize),
		m_DenseNodesSize(other.m_DenseNodesSize),
		m_Id(other.m_Id),
		m_Lat(other.m_Lat),
		m_Lon(other.m_Lon)
	{}

	void OSMNodeStream::next() {
		if (isNull())
			return;

		m_Position++;
		m_DensePosition = m_Position - m_NodesSize;

		if (m_DensePosition < 0) {
			m_Id = m_Controller->m_NodesGroup->nodes(m_Position).id();
			m_Lat = m_Controller->m_NodesGroup->nodes(m_Position).lat();
			m_Lon = m_Controller->m_NodesGroup->nodes(m_Position).lon();
		}
		else if (m_DensePosition > 0) {
			m_Id += m_Controller->m_DenseNodesGroup->dense().id(m_DensePosition);
			m_Lat += m_Controller->m_DenseNodesGroup->dense().lat(m_DensePosition);
			m_Lon += m_Controller->m_DenseNodesGroup->dense().lon(m_DensePosition);
		}
		else {
			m_Id = m_Controller->m_DenseNodesGroup->dense().id(0);
			m_Lat = m_Controller->m_DenseNodesGroup->dense().lat(0);
			m_Lon = m_Controller->m_DenseNodesGroup->dense().lon(0);
		}

		m_WGS84Lat = m_Lat * m_Controller->granularity() + m_Controller->latOffset();
		m_WGS84Lon = m_Lon * m_Controller->granularity() + m_Controller->lonOffset();
	}

	void OSMNodeStream::previous() {
		if (isNull())
			return;

		m_Position--;

		m_DensePosition = m_Position - m_NodesSize;
		if (m_DensePosition < 0) {
			m_Id = m_Controller->m_NodesGroup->nodes(m_Position).id();
			m_Lat = m_Controller->m_NodesGroup->nodes(m_Position).lat();
			m_Lon = m_Controller->m_NodesGroup->nodes(m_Position).lon();
		}
		else if (m_DensePosition > 0) {
			m_Id -= m_Controller->m_DenseNodesGroup->dense().id(m_DensePosition + 1);
			m_Lat -= m_Controller->m_DenseNodesGroup->dense().lat(m_DensePosition + 1);
			m_Lon -= m_Controller->m_DenseNodesGroup->dense().lon(m_DensePosition + 1);
		}
		else {
			m_Id = m_Controller->m_DenseNodesGroup->dense().id(0);
			m_Lat = m_Controller->m_DenseNodesGroup->dense().lat(0);
			m_Lon = m_Controller->m_DenseNodesGroup->dense().lon(0);
		}

		m_WGS84Lat = m_Lat * m_Controller->granularity() + m_Controller->latOffset();
		m_WGS84Lon = m_Lon * m_Controller->granularity() + m_Controller->lonOffset();
	}

	int OSMNodeStream::keysSize() const {
		return (m_DensePosition < 0) ?
			m_Controller->m_NodesGroup->nodes(m_Position).keys_size() :
			m_Controller->queryDenseNodeKeyValIndex(m_DensePosition * 2 + 1);
	}

	int OSMNodeStream::keyId(int index) const {
		if (index < 0 || index > keysSize())
			return -1;

		return (m_DensePosition < 0) ?
			m_Controller->m_NodesGroup->nodes(m_Position).keys(index) :
			m_Controller->m_DenseNodesGroup->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_DensePosition * 2) + index * 2);
	}

	int OSMNodeStream::valueId(int index) const {
		if (index < 0 || index > keysSize())
			return -1;

		return (m_DensePosition < 0) ?
			m_Controller->m_NodesGroup->nodes(m_Position).vals(index) :
			m_Controller->m_DenseNodesGroup->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_DensePosition * 2) + index * 2 + 1);
	}

	std::string OSMNodeStream::key(int index) const {
		int id = keyId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}

	std::string OSMNodeStream::value(int index) const {
		int id = valueId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}

// OSMPlainNodeAdaptor

	OSMNode::OSMPlainNodeAdaptor::OSMPlainNodeAdaptor() : AbstractOSMNodeAdaptor() {}
	OSMNode::OSMPlainNodeAdaptor::OSMPlainNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position) :
		AbstractOSMNodeAdaptor(controller, group, position) {}

	int64_t OSMNode::OSMPlainNodeAdaptor::id() {
		return m_Group->nodes(m_Position).id();
	}

	int64_t OSMNode::OSMPlainNodeAdaptor::lat() {
		return m_Group->nodes(m_Position).lat();
	}

	int64_t OSMNode::OSMPlainNodeAdaptor::lon() {
		return m_Group->nodes(m_Position).lon();
	}

	int OSMNode::OSMPlainNodeAdaptor::keysSize() const {
		return m_Group->nodes(m_Position).keys_size();
	}

	int OSMNode::OSMPlainNodeAdaptor::keyId(int index) const {
		return m_Group->nodes(m_Position).keys(index);
	}

	int OSMNode::OSMPlainNodeAdaptor::valueId(int index) const {
		return m_Group->nodes(m_Position).vals(index);
	}
	std::string OSMNode::OSMPlainNodeAdaptor::key(int index) const {
		return m_Controller->queryStringTable(m_Group->nodes(m_Position).keys(index));
	}

	std::string OSMNode::OSMPlainNodeAdaptor::value(int index) const {
		return m_Controller->queryStringTable(m_Group->nodes(m_Position).vals(index));
	}

// OSMDenseNodeAdaptor

	OSMNode::OSMDenseNodeAdaptor::OSMDenseNodeAdaptor() : AbstractOSMNodeAdaptor() {}
	OSMNode::OSMDenseNodeAdaptor::OSMDenseNodeAdaptor(OSMPrimitiveBlockController * controller, PrimitiveGroup * group, int position) :
		AbstractOSMNodeAdaptor(controller, group, position) {}

	int64_t OSMNode::OSMDenseNodeAdaptor::id() {
		if (m_Controller->m_DenseNodesUnpacked)
			return m_Group->dense().id(m_Position);

		if (!m_HasCachedId) {
			m_CachedId = m_Group->dense().id(0);
			for (int i = 0; i < m_Position; i++)
				m_CachedId += m_Group->dense().id(i);
		}

		return m_CachedId;
	}

	int64_t OSMNode::OSMDenseNodeAdaptor::lat() {
		if (m_Controller->m_DenseNodesUnpacked)
			return m_Group->dense().lat(m_Position);

		if (!m_HasCachedLat) {
			m_CachedLat = m_Group->dense().lat(0);
			for (int i = 0; i < m_Position; i++)
				m_CachedLat += m_Group->dense().lat(i);
		}

		return m_CachedLat;
	}

	int64_t OSMNode::OSMDenseNodeAdaptor::lon() {
		if (m_Controller->m_DenseNodesUnpacked)
			return m_Group->dense().lon(m_Position);

		if (!m_HasCachedLon) {
			m_CachedLon = m_Group->dense().lon(0);
			for (int i = 0; i < m_Position; i++)
				m_CachedLon += m_Group->dense().lon(i);
		}

		return m_CachedLon;
	}

	int OSMNode::OSMDenseNodeAdaptor::keysSize() const {
		return (!m_Group->dense().keys_vals_size()) ? 0 : m_Controller->queryDenseNodeKeyValIndex(m_Position * 2 + 1);
	}

	int OSMNode::OSMDenseNodeAdaptor::keyId(int index) const {
		return (index < 0 || index >= keysSize()) ? -1 : m_Group->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_Position * 2) + index * 2);
	}

	int OSMNode::OSMDenseNodeAdaptor::valueId(int index) const {
		return (index < 0 || index >= keysSize()) ? -1 : m_Group->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_Position * 2) + index * 2 + 1);
	}

	std::string OSMNode::OSMDenseNodeAdaptor::key(int index) const {
		int id = keyId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}

	std::string OSMNode::OSMDenseNodeAdaptor::value(int index) const {
		int id = valueId(index);
		return (id < 0) ? std::string() : m_Controller->queryStringTable(id);
	}
}
