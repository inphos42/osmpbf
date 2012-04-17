#include "osmdatacontroller.h"

#include <iostream>

#include "osmformat.pb.h"
#include "osmblobfile.h"

namespace osmpbf {

// OSMPrimitiveBlockController

	OSMPrimitiveBlockController::OSMPrimitiveBlockController(char * rawData, uint32_t length, bool unpackDense) :
		m_NodesGroup(NULL),
		m_DenseNodesGroup(NULL),
		m_WaysGroup(NULL),
		m_RelationsGroup(NULL),
		m_DenseNodesUnpacked(false),
		m_DenseNodeKeyValIndex(NULL)
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;

		m_PBFPrimitiveBlock = new PrimitiveBlock();

		if (m_PBFPrimitiveBlock->ParseFromArray((void*)rawData, length)) {
			// we assume each primitive block has one primitive group for each primitive type
			// populate group refs
			PrimitiveGroup ** primGroups = m_PBFPrimitiveBlock->mutable_primitivegroup()->mutable_data();

			for (int i = 0; i < m_PBFPrimitiveBlock->primitivegroup_size(); ++i) {
				if (primGroups[i]->nodes_size()) {
					m_NodesGroup = primGroups[i];
					break;
				}

				if (primGroups[i]->has_dense()) {
					m_DenseNodesGroup = primGroups[i];

					if (unpackDense) unpackDenseNodes();

					break;
				}

				if (primGroups[i]->ways_size()) {
					m_WaysGroup = primGroups[i];
					break;
				}

				if (primGroups[i]->relations_size()) {
					m_RelationsGroup = primGroups[i];
					break;
				}
			}

			// successfully return
			return;
		}

		std::cerr << "ERROR: invalid OSM primitive block" << std::endl;

		delete m_PBFPrimitiveBlock;
		m_PBFPrimitiveBlock = NULL;
	}

	OSMPrimitiveBlockController::~OSMPrimitiveBlockController() {
		delete m_PBFPrimitiveBlock;

		if (m_DenseNodeKeyValIndex)
			delete[] m_DenseNodeKeyValIndex;
	}

	OSMNode OSMPrimitiveBlockController::getNodeAt(int position) const {
		if (!m_NodesGroup && !m_DenseNodesGroup)
			return OSMNode();

		if (m_DenseNodesGroup && (!m_NodesGroup || (position > m_NodesGroup->nodes_size())))
			return OSMNode(new OSMProtoBufDenseNode(const_cast<OSMPrimitiveBlockController *>(this), m_DenseNodesGroup, position));
		else
			return OSMNode(new OSMProtoBufNode(const_cast<OSMPrimitiveBlockController *>(this), m_NodesGroup, position));

	}

	int OSMPrimitiveBlockController::nodesSize() const {
		int result = 0;

		if (m_NodesGroup)
			result += m_NodesGroup->nodes_size();

		if (m_DenseNodesGroup)
			result += m_DenseNodesGroup->dense().id_size();

		return result;
	}

	OSMWay OSMPrimitiveBlockController::getWayAt(int position) const {
		return OSMWay(new OSMProtoBufWay(const_cast<OSMPrimitiveBlockController *>(this), m_WaysGroup, position));
	}

	int OSMPrimitiveBlockController::waysSize() const {
		if (m_WaysGroup)
			return m_WaysGroup->ways_size();
		else
			return 0;
	}

	inline int32_t OSMPrimitiveBlockController::granularity() const {
		return m_PBFPrimitiveBlock->granularity();
	}

	inline int64_t OSMPrimitiveBlockController::latOffset() const {
		return m_PBFPrimitiveBlock->lat_offset();
	}

	inline int64_t OSMPrimitiveBlockController::lonOffset() const {
		return m_PBFPrimitiveBlock->lon_offset();
	}

	void OSMPrimitiveBlockController::unpackDenseNodes(){
		if (!m_DenseNodesGroup)
			return;

		m_DenseNodesUnpacked = true;

		int64_t id = m_DenseNodesGroup->dense().id(0);
		int64_t lat = m_DenseNodesGroup->dense().lat(0);
		int64_t lon = m_DenseNodesGroup->dense().lon(0);

		for (int i = 1; i < m_DenseNodesGroup->dense().id_size(); i++) {
			id += m_DenseNodesGroup->dense().id(i);
			lat += m_DenseNodesGroup->dense().lat(i);
			lon += m_DenseNodesGroup->dense().lon(i);

			m_DenseNodesGroup->mutable_dense()->mutable_id()->Set(i, id);
			m_DenseNodesGroup->mutable_dense()->mutable_lat()->Set(i, lat);
			m_DenseNodesGroup->mutable_dense()->mutable_lon()->Set(i, lon);
		}
	}

	void OSMPrimitiveBlockController::buildDenseNodeKeyValIndex() {
		int keys_vals_size = m_DenseNodesGroup->dense().keys_vals_size();

		if (!keys_vals_size)
			return;

		m_DenseNodeKeyValIndex = new int[m_DenseNodesGroup->dense().id_size() * 2];

		int keyvalPos = 0;
		int keyvalLength = 0;

		int i = 0;
		while(i < keys_vals_size) {
			if (m_DenseNodesGroup->dense().keys_vals(i)) {
				keyvalLength++;
				i++;
			}
			else {
				m_DenseNodeKeyValIndex[keyvalPos * 2] = i - (keyvalLength * 2);
				m_DenseNodeKeyValIndex[keyvalPos * 2 + 1] = keyvalLength;
				keyvalPos++;
				keyvalLength = 0;
			}

			i++;
		}
	}

	inline std::string OSMPrimitiveBlockController::queryStringTable(int id) const {
		return m_PBFPrimitiveBlock->stringtable().s(id);
	}

// OSMProtoBufNode

	int64_t OSMPrimitiveBlockController::OSMProtoBufNode::id() {
		return m_Group->nodes(m_Position).id();
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufNode::lat() {
		return m_Group->nodes(m_Position).lat();
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufNode::lon() {
		return m_Group->nodes(m_Position).lon();
	}

	int OSMPrimitiveBlockController::OSMProtoBufNode::keysSize() {
		return m_Group->nodes(m_Position).keys_size();
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufNode::key(int index) {
		return m_Controller->queryStringTable(
			m_Group->nodes(m_Position).keys(index));
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufNode::value(int index) {
		return m_Controller->queryStringTable(
			m_Group->nodes(m_Position).vals(index));
	}

	// TODO
	std::string OSMPrimitiveBlockController::OSMProtoBufNode::value(std::string key) {
		return std::string();
	}

// OSMProtoBufDenseNode

	int64_t OSMPrimitiveBlockController::OSMProtoBufDenseNode::id() {
		if (m_Controller->m_DenseNodesUnpacked)
			return m_Group->dense().id(m_Position);

		if (!m_HasCachedId) {
			m_CachedId = m_Group->dense().id(0);
			for (int i = 0; i < m_Position; i++)
				m_CachedId += m_Group->dense().id(i);
		}

		return m_CachedId;
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufDenseNode::lat() {
		if (m_Controller->m_DenseNodesUnpacked)
			return m_Group->dense().lat(m_Position);

		if (!m_HasCachedLat) {
			m_CachedLat = m_Group->dense().lat(0);
			for (int i = 0; i < m_Position; i++)
				m_CachedLat += m_Group->dense().lat(i);
		}

		return m_CachedLat;
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufDenseNode::lon() {
		if (m_Controller->m_DenseNodesUnpacked)
			return m_Group->dense().lon(m_Position);

		if (!m_HasCachedLon) {
			m_CachedLon = m_Group->dense().lon(0);
			for (int i = 0; i < m_Position; i++)
				m_CachedLon += m_Group->dense().lon(i);
		}

		return m_CachedLon;
	}

	int OSMPrimitiveBlockController::OSMProtoBufDenseNode::keysSize() {
		if (!m_Group->dense().keys_vals_size())
			return 0;

		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();

		return m_Controller->m_DenseNodeKeyValIndex[m_Position * 2 + 1];
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufDenseNode::key(int index) {
		if (!m_Group->dense().keys_vals_size() || index < 0 || index >= keysSize())
			return std::string();

		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();

		return m_Controller->queryStringTable(m_Group->dense().keys_vals(m_Controller->m_DenseNodeKeyValIndex[m_Position * 2] + index * 2));
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufDenseNode::value(int index) {
		if (!m_Group->dense().keys_vals_size() || index < 0 || index >= keysSize())
			return std::string();

		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();;

		return m_Controller->queryStringTable(m_Group->dense().keys_vals(m_Controller->m_DenseNodeKeyValIndex[m_Position * 2] + index * 2 + 1));
	}

	// TODO
	std::string OSMPrimitiveBlockController::OSMProtoBufDenseNode::value(std::string key) {
		return std::string();
	}

// OSMProtoBufWay

	int64_t OSMPrimitiveBlockController::OSMProtoBufWay::id() {
		return m_Group->ways(m_Position).id();
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufWay::ref(int index) {
		int64_t result = m_Group->ways(m_Position).refs(0);
		for (int i = 0; i < index; i++)
			result += m_Group->ways(m_Position).refs(i);

		return result;
	}

	int OSMPrimitiveBlockController::OSMProtoBufWay::refsSize() const {
		return m_Group->ways(m_Position).refs_size();
	}

	int OSMPrimitiveBlockController::OSMProtoBufWay::keysSize() {
		return m_Group->ways(m_Position).keys_size();
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufWay::key(int index) {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).keys(index));
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufWay::value(int index) {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).vals(index));
	}

	// TODO
	std::string OSMPrimitiveBlockController::OSMProtoBufWay::value(std::string key) {
		return std::string();
	}
}
