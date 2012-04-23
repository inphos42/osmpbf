#include "osmnode.h"

#include "osmdatacontroller.h"
#include "osmformat.pb.h"

namespace osmpbf {
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

	int OSMNode::OSMPlainNodeAdaptor::keysSize() {
		return m_Group->nodes(m_Position).keys_size();
	}

	std::string OSMNode::OSMPlainNodeAdaptor::key(int index) {
		return m_Controller->queryStringTable(
			m_Group->nodes(m_Position).keys(index));
	}

	std::string OSMNode::OSMPlainNodeAdaptor::value(int index) {
		return m_Controller->queryStringTable(
			m_Group->nodes(m_Position).vals(index));
	}

// 	// TODO
// 	std::string OSMNode::OSMPlainNodeAdaptor::value(std::string key) {
// 		return std::string();
// 	}

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

	int OSMNode::OSMDenseNodeAdaptor::keysSize() {
		if (!m_Group->dense().keys_vals_size())
			return 0;

		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();

		return m_Controller->m_DenseNodeKeyValIndex[m_Position * 2 + 1];
	}

	std::string OSMNode::OSMDenseNodeAdaptor::key(int index) {
		if (!m_Group->dense().keys_vals_size() || index < 0 || index >= keysSize())
			return std::string();

		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();

		return m_Controller->queryStringTable(m_Group->dense().keys_vals(m_Controller->m_DenseNodeKeyValIndex[m_Position * 2] + index * 2));
	}

	std::string OSMNode::OSMDenseNodeAdaptor::value(int index) {
		if (!m_Group->dense().keys_vals_size() || index < 0 || index >= keysSize())
			return std::string();

		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();;

		return m_Controller->queryStringTable(m_Group->dense().keys_vals(m_Controller->m_DenseNodeKeyValIndex[m_Position * 2] + index * 2 + 1));
	}

// 	// TODO
// 	std::string OSMNode::OSMDenseNodeAdaptor::value(std::string key) {
// 		return std::string();
// 	}
}
