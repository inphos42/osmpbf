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
			return OSMNode(new OSMNode::OSMDenseNodeAdaptor(const_cast<OSMPrimitiveBlockController *>(this), m_DenseNodesGroup, position));
		else
			return OSMNode(new OSMNode::OSMPlainNodeAdaptor(const_cast<OSMPrimitiveBlockController *>(this), m_NodesGroup, position));

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
		return OSMWay(new OSMWay::OSMWayAdaptor(const_cast<OSMPrimitiveBlockController *>(this), m_WaysGroup, position));
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

	std::string OSMPrimitiveBlockController::queryStringTable(int id) const {
		return m_PBFPrimitiveBlock->stringtable().s(id);
	}

	OSMNodeStream OSMPrimitiveBlockController::getNodeStream() {
		return OSMNodeStream(this);
	}

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
		if (m_Position >= m_NodesSize + m_DenseNodesSize)
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
	}

	void OSMNodeStream::previous() {
		if (m_Position < 1)
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
	}

	int OSMNodeStream::keysSize() const {
		if (m_DensePosition < 0)
			return m_Controller->m_NodesGroup->nodes(m_Position).keys_size();
		else
			return m_Controller->m_DenseNodeKeyValIndex[m_DensePosition * 2 + 1];
	}

	std::string OSMNodeStream::key(int index) const {
		if (index < 0 || index > keysSize())
			return std::string();

		int resultId = (m_DensePosition < 0) ?
			m_Controller->m_NodesGroup->nodes(m_Position).keys(index) :
			m_Controller->m_DenseNodesGroup->dense().keys_vals(m_Controller->m_DenseNodeKeyValIndex[m_DensePosition * 2] + index * 2);

		return m_Controller->queryStringTable(resultId);
	}

	std::string OSMNodeStream::value(int index) const {
		if (index < 0 || index > keysSize())
			return std::string();

		int resultId = (m_DensePosition < 0) ?
			m_Controller->m_NodesGroup->nodes(m_Position).vals(index) :
			m_Controller->m_DenseNodesGroup->dense().keys_vals(m_Controller->m_DenseNodeKeyValIndex[m_DensePosition * 2] + index * 2 + 1);

		return m_Controller->queryStringTable(resultId);
	}
}
