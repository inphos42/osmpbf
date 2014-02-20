/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2014 Oliver Groß.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <http://www.gnu.org/licenses/>.
 */

#include "primitiveblockinputadaptor.h"

#include <cstddef>
#include <iostream>

#include "osmformat.pb.h"

#include "inode.h"
#include "iway.h"
#include "irelation.h"

#include "assert.h"

namespace osmpbf {

// PrimitiveBlockInputAdaptor

	PrimitiveBlockInputAdaptor::PrimitiveBlockInputAdaptor() :
		m_PrimitiveBlock(NULL),
		m_PlainNodesGroup(NULL),
		m_DenseNodesGroup(NULL),
		m_WaysGroup(NULL),
		m_RelationsGroup(NULL),
		m_DenseNodesUnpacked(false),
		m_DenseNodeKeyValIndex(NULL)
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}

	PrimitiveBlockInputAdaptor::PrimitiveBlockInputAdaptor(char * rawData, OffsetType length, bool unpackDense) :
		m_PrimitiveBlock(NULL),
		m_PlainNodesGroup(NULL),
		m_DenseNodesGroup(NULL),
		m_WaysGroup(NULL),
		m_RelationsGroup(NULL),
		m_DenseNodesUnpacked(false),
		m_DenseNodeKeyValIndex(NULL)
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;

		parseData(rawData, length, unpackDense);
	}

	PrimitiveBlockInputAdaptor::~PrimitiveBlockInputAdaptor() {
		delete m_PrimitiveBlock;
		delete[] m_DenseNodeKeyValIndex;
	}

	void PrimitiveBlockInputAdaptor::parseData(char * rawData, OffsetType length, bool unpackDense) {
		delete m_PrimitiveBlock;
		delete[] m_DenseNodeKeyValIndex;

		m_PlainNodesGroup = NULL;
		m_DenseNodesGroup = NULL;
		m_WaysGroup = NULL;
		m_RelationsGroup = NULL;
		m_DenseNodesUnpacked = false;
		m_DenseNodeKeyValIndex = NULL;

		m_PrimitiveBlock = new crosby::binary::PrimitiveBlock();

		if (m_PrimitiveBlock->ParseFromArray((void*)rawData, length)) {
			// we assume each primitive block has one primitive group for each primitive type
			// populate group refs
			crosby::binary::PrimitiveGroup ** primGroups = m_PrimitiveBlock->mutable_primitivegroup()->mutable_data();

			for (int i = 0; i < m_PrimitiveBlock->primitivegroup_size(); ++i) {
				if (primGroups[i]->nodes_size()) {
					if (m_PlainNodesGroup) {
						std::cerr << "WARNING: second plain nodes group found. using first" << std::endl;
						break;
					}
					m_PlainNodesGroup = primGroups[i];
				}

				if (primGroups[i]->has_dense()) {
					if (m_DenseNodesGroup) {
						std::cerr << "WARNING: second dense nodes group found. using first" << std::endl;
						break;
					}
					m_DenseNodesGroup = primGroups[i];

					if (unpackDense) unpackDenseNodes();
				}

				if (primGroups[i]->ways_size()) {
					if (m_WaysGroup) {
						std::cerr << "WARNING: second ways group found. using first" << std::endl;
						break;
					}
					m_WaysGroup = primGroups[i];
				}

				if (primGroups[i]->relations_size()) {
					if (m_RelationsGroup) {
						std::cerr << "WARNING: second relations group found. using first" << std::endl;
						break;
					}
					m_RelationsGroup = primGroups[i];
				}
			}

			// successfully return
			return;
		}

		std::cerr << "ERROR: invalid OSM primitive block" << std::endl;
		if (!m_PrimitiveBlock->has_stringtable())
			std::cerr << "no stringtable field found" << std::endl;

		delete m_PrimitiveBlock;
		m_PrimitiveBlock = NULL;
	}

	INode PrimitiveBlockInputAdaptor::getNodeAt(int position) const {
		if (!m_PlainNodesGroup && !m_DenseNodesGroup)
			return INode();

		if (m_DenseNodesGroup && (!m_PlainNodesGroup || (position > m_PlainNodesGroup->nodes_size())))
			return INode(new DenseNodeInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_DenseNodesGroup->dense(), position));
		else
			return INode(new PlainNodeInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_PlainNodesGroup->nodes(position)));

	}

	int PrimitiveBlockInputAdaptor::nodesSize(unsigned char type) const {
		int result = 0;

		if (m_PlainNodesGroup && (type & PlainNode))
			result += m_PlainNodesGroup->nodes_size();

		if (m_DenseNodesGroup && (type & DenseNode))
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

	int PrimitiveBlockInputAdaptor::relationsSize() const {
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

		assert(keyvalPos == m_DenseNodesGroup->dense().id_size());
	}

	const std::string & PrimitiveBlockInputAdaptor::queryStringTable(int id) const {
		return m_PrimitiveBlock->stringtable().s(id);
	}

	int PrimitiveBlockInputAdaptor::stringTableSize() const {
		return m_PrimitiveBlock->stringtable().s_size();
	}

	int PrimitiveBlockInputAdaptor::findString(const std::string & str) const {
		if (isNull())
			return 0;

		int size = stringTableSize();

		for (int id = 1; id < size; ++id) {
			if (str == queryStringTable(id))
				return id;
		}

		return 0;
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
