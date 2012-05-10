#include "osmdatacontroller.h"

#include <iostream>

#include "osmformat.pb.h"
#include "osmblobfile.h"

namespace osmpbf {

// OSMPrimitiveBlockInputAdaptor

	OSMPrimitiveBlockInputAdaptor::OSMPrimitiveBlockInputAdaptor(char * rawData, uint32_t length, bool unpackDense) :
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

	OSMPrimitiveBlockInputAdaptor::~OSMPrimitiveBlockInputAdaptor() {
		delete m_PBFPrimitiveBlock;

		if (m_DenseNodeKeyValIndex)
			delete[] m_DenseNodeKeyValIndex;
	}

	OSMNode OSMPrimitiveBlockInputAdaptor::getNodeAt(int position) const {
		if (!m_NodesGroup && !m_DenseNodesGroup)
			return OSMNode();

		if (m_DenseNodesGroup && (!m_NodesGroup || (position > m_NodesGroup->nodes_size())))
			return OSMNode(new OSMDenseNodeAdaptor(const_cast<OSMPrimitiveBlockInputAdaptor *>(this), m_DenseNodesGroup, position));
		else
			return OSMNode(new OSMPlainNodeAdaptor(const_cast<OSMPrimitiveBlockInputAdaptor *>(this), m_NodesGroup, position));

	}

	int OSMPrimitiveBlockInputAdaptor::nodesSize() const {
		int result = 0;

		if (m_NodesGroup)
			result += m_NodesGroup->nodes_size();

		if (m_DenseNodesGroup)
			result += m_DenseNodesGroup->dense().id_size();

		return result;
	}

	OSMWay OSMPrimitiveBlockInputAdaptor::getWayAt(int position) const {
		return OSMWay(new OSMWayAdaptor(const_cast<OSMPrimitiveBlockInputAdaptor *>(this), m_WaysGroup, position));
	}

	int OSMPrimitiveBlockInputAdaptor::waysSize() const {
		if (m_WaysGroup)
			return m_WaysGroup->ways_size();
		else
			return 0;
	}

	int32_t OSMPrimitiveBlockInputAdaptor::granularity() const {
		return m_PBFPrimitiveBlock->granularity();
	}

	int64_t OSMPrimitiveBlockInputAdaptor::latOffset() const {
		return m_PBFPrimitiveBlock->lat_offset();
	}

	int64_t OSMPrimitiveBlockInputAdaptor::lonOffset() const {
		return m_PBFPrimitiveBlock->lon_offset();
	}

	void OSMPrimitiveBlockInputAdaptor::unpackDenseNodes(){
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

	void OSMPrimitiveBlockInputAdaptor::buildDenseNodeKeyValIndex() {
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

	std::string OSMPrimitiveBlockInputAdaptor::queryStringTable(int id) const {
		return m_PBFPrimitiveBlock->stringtable().s(id);
	}

	int OSMPrimitiveBlockInputAdaptor::stringTableSize() const {
		return m_PBFPrimitiveBlock->stringtable().s_size();
	}

	OSMStreamNode OSMPrimitiveBlockInputAdaptor::getNodeStream() {
		return OSMStreamNode(this);
	}

	OSMStreamWay OSMPrimitiveBlockInputAdaptor::getWayStream() {
		return OSMStreamWay(this);
	}
}
