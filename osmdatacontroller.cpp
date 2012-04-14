#include "osmdatacontroller.h"

#include "osmformat.pb.h"
#include "osmblobfile.h"

namespace osmpbf {
	OSMPrimitiveBlockController::OSMPrimitiveBlockController(char * rawData, uint32_t length) :
		m_NodesGroup(NULL),
		m_DenseNodesGroup(NULL),
		m_WaysGroup(NULL),
		m_RelationsGroup(NULL)
	{
		m_PBFPrimitiveBlock = new PrimitiveBlock();
		
		if (m_PBFPrimitiveBlock->ParseFromArray((void*)rawData, length)) {
			// populate grous refs
			const PrimitiveGroup * const * primGroups = m_PBFPrimitiveBlock->primitivegroup().data();
			
			for (int i = 0; i < m_PBFPrimitiveBlock->primitivegroup_size(); ++i) {
				if (primGroups[i]->nodes_size()) {
					m_NodesGroup = primGroups[i];
					break;
				}
				
				if (primGroups[i]->has_dense()) {
					m_DenseNodesGroup = primGroups[i];
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
		
		cerr << "ERROR: invalid OSM primitive block" << endl;
		
		delete m_PBFPrimitiveBlock;
		m_PBFPrimitiveBlock = NULL;
	}
	
	OSMPrimitiveBlockController::~OSMPrimitiveBlockController() {
		delete m_PBFPrimitiveBlock;
	}

	OSMNode OSMPrimitiveBlockController::getNode(int position) {
		if (!m_NodesGroup && !m_DenseNodesGroup)
			return OSMNode();
		
		bool isDense = m_DenseNodesGroup &&
			(!m_NodesGroup || (position > m_NodesGroup->nodes_size()));
		
		return OSMNode(isDense ? (new OSMProtoBufDenseNode(this, m_DenseNodesGroup, position)) :
			(new OSMProtoBufNode(this, m_NodesGroup, position)));
	}
	
	// TODO
	OSMNode OSMPrimitiveBlockController::getNode(int64_t id) {
		return OSMNode();
	}
	
	int OSMPrimitiveBlockController::nodeCount() {
		int result = 0;
		
		if (m_NodesGroup)
			result += m_NodesGroup->nodes_size();
		
		if (m_DenseNodesGroup)
			result += m_DenseNodesGroup->dense().id_size();
		
		return result;
	}
	
	OSMWay OSMPrimitiveBlockController::getWay(int position) {
		return OSMWay(new OSMProtoBufWay(this, m_WaysGroup, position));
	}
	
	// TODO
	OSMWay OSMPrimitiveBlockController::getWay(int64_t id) {
		return OSMWay();
	}
	
	int OSMPrimitiveBlockController::wayCount() {
		if (m_WaysGroup)
			return m_WaysGroup->ways_size();
		else
			return 0;
	}
	
	inline int32_t OSMPrimitiveBlockController::granularity() {
		return m_PBFPrimitiveBlock->granularity();
	}

	inline int64_t OSMPrimitiveBlockController::latOffset() {
		return m_PBFPrimitiveBlock->lat_offset();
	}

	inline int64_t OSMPrimitiveBlockController::lonOffset() {
		return m_PBFPrimitiveBlock->lon_offset();
	}
	
	void OSMPrimitiveBlockController::buildDenseNodeKeyValIndex() {
		int keys_vals_size = m_PBFPrimitiveBlock->primitivegroup(m_GroupIndex).dense().keys_vals_size();
		
		if (!keys_vals_size)
			return;
		
		m_DenseNodeKeyValIndex = new int[m_PBFPrimitiveBlock->primitivegroup(m_GroupIndex).dense().id_size() * 2];
		
		int keyvalPos = 0;
		int keyvalLength = 0;
		
		int i = 0;
		while(i < keys_vals_size) {
			if (m_PBFPrimitiveBlock->primitivegroup(m_GroupIndex).dense().keys_vals(i)) {
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
	
	std::string OSMPrimitiveBlockController::queryStringTable(int id) {
		return m_PBFPrimitiveBlock->stringtable().s(id);
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufNode::id() const {
		return m_Group->nodes(m_Position).id();
	}
	
	int64_t OSMPrimitiveBlockController::OSMProtoBufNode::lat() const {
		return m_Group->nodes(m_Position).lat();
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufNode::lon() const {
		return m_Group->nodes(m_Position).lon();
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufNode::key(int index) {
		return m_Controller->queryStringTable(
			m_Group->nodes(m_Position).keys(index));
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufDenseNode::value(int index) {
		return m_Controller->queryStringTable(
			m_Group->nodes(m_Position).vals(index));
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufDenseNode::id() const {
		return m_Group->dense().id(m_Position);
	}
	
	int64_t OSMPrimitiveBlockController::OSMProtoBufDenseNode::lat() const {
		return m_Group->dense().lat(m_Position);
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufDenseNode::lon() const {
		return m_Group->dense().lon(m_Position);
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufDenseNode::key(int index) {
		if (!m_Group->dense().keys_vals_size())
			return std::string();
		
		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();
		
		m_Controller->queryStringTable(m_DenseNodeKeyValIndex[m_Position * 2]);
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufDenseNode::value(int index) {
		if (!m_Group->dense().keys_vals_size())
			return std::string();
		
		if (!m_Controller->m_DenseNodeKeyValIndex)
			m_Controller->buildDenseNodeKeyValIndex();
		
		m_Controller->queryStringTable(m_DenseNodeKeyValIndex[m_Position * 2 + 1]);
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufWay::id() const {
		return m_Group->ways(m_Position).id();
	}

	int64_t OSMPrimitiveBlockController::OSMProtoBufWay::ref(int index) const {
		return m_Group->ways(m_Position).refs(index);
	}

	int OSMPrimitiveBlockController::OSMProtoBufWay::refSize() const {
		return m_Group->ways(m_Position).refs_size();
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufWay::key(int index) {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).keys(index));
	}

	std::string OSMPrimitiveBlockController::OSMProtoBufWay::value(int index) {
		return m_Controller->queryStringTable(m_Group->ways(m_Position).vals(index));
	}
	
// 	// TODO
// 	OSMDataController::OSMDataController(OSMBlobFile * rawData, AbstractOSMDataIndex * index) {
// 	}
// 	
// 	// TODO
// 	OSMNode OSMDataController::getNode(int position) {
// 		return OSMNode();
// 	}
// 	
// 	// TODO
// 	OSMWay OSMDataController::getWay(int position){
// 		return OSMWay();
// 	}
}
