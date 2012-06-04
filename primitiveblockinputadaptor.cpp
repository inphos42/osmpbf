#include "primitiveblockinputadaptor.h"

#include <iostream>

#include "osmformat.pb.h"
#include "blobfile.h"

#include "inode.h"
#include "iway.h"
#include "irelation.h"

namespace osmpbf {

// PrimitiveBlockInputAdaptor

	PrimitiveBlockInputAdaptor::PrimitiveBlockInputAdaptor(char * rawData, uint32_t length, bool unpackDense) :
		m_PlainNodesGroup(NULL),
		m_DenseNodesGroup(NULL),
		m_WaysGroup(NULL),
		m_RelationsGroup(NULL),
		m_DenseNodesUnpacked(false),
		m_DenseNodeKeyValIndex(NULL)
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;

		m_PrimitiveBlock = new PrimitiveBlock();

		if (m_PrimitiveBlock->ParseFromArray((void*)rawData, length)) {
			// we assume each primitive block has one primitive group for each primitive type
			// populate group refs
			PrimitiveGroup ** primGroups = m_PrimitiveBlock->mutable_primitivegroup()->mutable_data();

			for (int i = 0; i < m_PrimitiveBlock->primitivegroup_size(); ++i) {
				if (primGroups[i]->nodes_size()) {
					m_PlainNodesGroup = primGroups[i];
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

		delete m_PrimitiveBlock;
		m_PrimitiveBlock = NULL;
	}

	PrimitiveBlockInputAdaptor::~PrimitiveBlockInputAdaptor() {
		delete m_PrimitiveBlock;
		delete[] m_DenseNodeKeyValIndex;
	}

	INode PrimitiveBlockInputAdaptor::getNodeAt(int position) const {
		if (!m_PlainNodesGroup && !m_DenseNodesGroup)
			return INode();

		if (m_DenseNodesGroup && (!m_PlainNodesGroup || (position > m_PlainNodesGroup->nodes_size())))
			return INode(new DenseNodeInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_DenseNodesGroup->dense(), position));
		else
			return INode(new PlainNodeInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_PlainNodesGroup->nodes(position)));

	}

	int PrimitiveBlockInputAdaptor::nodesSize() const {
		int result = 0;

		if (m_PlainNodesGroup)
			result += m_PlainNodesGroup->nodes_size();

		if (m_DenseNodesGroup)
			result += m_DenseNodesGroup->dense().id_size();

		return result;
	}

	IWay PrimitiveBlockInputAdaptor::getWayAt(int position) const {
		return IWay(new WayInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_WaysGroup->ways().data()[position]));
	}

	int PrimitiveBlockInputAdaptor::waysSize() const {
		return m_WaysGroup ? m_WaysGroup->ways_size() : 0;
	}

	IRelation PrimitiveBlockInputAdaptor::getRelationAt(int position) const {
		return IRelation(new RelationInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_RelationsGroup->relations().data()[position]));
	}

	int PrimitiveBlockInputAdaptor::realtionsSize() const {
		return m_RelationsGroup ? m_RelationsGroup->relations_size() : 0;
	}

	int32_t PrimitiveBlockInputAdaptor::granularity() const {
		return m_PrimitiveBlock->granularity();
	}

	int64_t PrimitiveBlockInputAdaptor::latOffset() const {
		return m_PrimitiveBlock->lat_offset();
	}

	int64_t PrimitiveBlockInputAdaptor::lonOffset() const {
		return m_PrimitiveBlock->lon_offset();
	}

	void PrimitiveBlockInputAdaptor::unpackDenseNodes(){
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

	void PrimitiveBlockInputAdaptor::buildDenseNodeKeyValIndex() {
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

	const std::string & PrimitiveBlockInputAdaptor::queryStringTable(int id) const {
		return m_PrimitiveBlock->stringtable().s(id);
	}

	int PrimitiveBlockInputAdaptor::stringTableSize() const {
		return m_PrimitiveBlock->stringtable().s_size();
	}

	INodeStream PrimitiveBlockInputAdaptor::getNodeStream() {
		return INodeStream(this);
	}

	IWayStream PrimitiveBlockInputAdaptor::getWayStream() {
		return IWayStream(this);
	}

	IRelationStream PrimitiveBlockInputAdaptor::getRelationStream() {
		return IRelationStream(this);
	}

}
