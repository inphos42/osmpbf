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

#include <cstddef>
#include <iostream>

#include <osmpbf/primitiveblockinputadaptor.h>
#include <osmpbf/inode.h>
#include <osmpbf/iway.h>
#include <osmpbf/irelation.h>

#include <osmpbf/nodestreaminputadaptor.h>

#include "osmformat.pb.h"

namespace osmpbf
{

// PrimitiveBlockInputAdaptor

PrimitiveBlockInputAdaptor::PrimitiveBlockInputAdaptor() :
	m_PrimitiveBlock(nullptr),
	m_PlainNodesCount(0),
	m_DenseNodesCount(0),
	m_WaysCount(0),
	m_RelationsCount(0)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;
}

PrimitiveBlockInputAdaptor::PrimitiveBlockInputAdaptor(char * rawData, OffsetType length, bool unpackDense) :
	m_PrimitiveBlock(nullptr),
	m_PlainNodesCount(0),
	m_DenseNodesCount(0),
	m_WaysCount(0),
	m_RelationsCount(0)
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	parseData(rawData, length, unpackDense);
}

PrimitiveBlockInputAdaptor::~PrimitiveBlockInputAdaptor()
{
	delete m_PrimitiveBlock;
}

void PrimitiveBlockInputAdaptor::parseData(char * rawData, OffsetType length, bool unpackDense)
{
	delete m_PrimitiveBlock;

	m_PlainNodesGroups.clear();
	m_DenseNodesGroups.clear();
	m_WaysGroups.clear();
	m_RelationsGroups.clear();

	m_PrimitiveBlock = new crosby::binary::PrimitiveBlock();

	if (m_PrimitiveBlock->ParseFromArray((void*)rawData, length))
	{
		// we assume each primitive block has one primitive group for each primitive type
		// populate group refs
		crosby::binary::PrimitiveGroup ** primGroups = m_PrimitiveBlock->mutable_primitivegroup()->mutable_data();

		for (int i = 0; i < m_PrimitiveBlock->primitivegroup_size(); ++i) {
			if (primGroups[i]->nodes_size()) {
				m_PlainNodesCount += primGroups[i]->nodes_size();
				m_PlainNodesGroups.push_back(primGroups[i]);
			}

			if (primGroups[i]->has_dense()) {
				m_DenseNodesCount += primGroups[i]->dense().id_size();
				m_DenseNodesGroups.push_back(DenseNodesData(primGroups[i], unpackDense));
			}

			if (primGroups[i]->ways_size()) {
				m_WaysCount += primGroups[i]->ways_size();
				m_WaysGroups.push_back(primGroups[i]);
			}

			if (primGroups[i]->relations_size()) {
				m_RelationsCount += primGroups[i]->relations_size();
				m_RelationsGroups.push_back(primGroups[i]);
			}
		}
	}
	else
	{
		std::cerr << "ERROR: invalid OSM primitive block" << std::endl;
		if (!m_PrimitiveBlock->has_stringtable())
			std::cerr << "no stringtable field found" << std::endl;

		delete m_PrimitiveBlock;
		m_PrimitiveBlock = NULL;
	}
}

//	INode PrimitiveBlockInputAdaptor::getNodeAt(int position) const {
//		if (!m_PlainNodesGroup && !m_DenseNodesGroup)
//			return INode();

//		if (m_DenseNodesGroup && (!m_PlainNodesGroup || (position > m_PlainNodesGroup->nodes_size())))
//			return INode(new DenseNodeInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_DenseNodesGroup->dense(), position));
//		else
//			return INode(new PlainNodeInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_PlainNodesGroup->nodes(position)));

//	}

int PrimitiveBlockInputAdaptor::nodesSize(unsigned char type) const
{
	int result = 0;

	if (type & PlainNode)
		result += m_PlainNodesCount;

	if (type & DenseNode)
		result += m_DenseNodesCount;

	return result;
}

//	IWay PrimitiveBlockInputAdaptor::getWayAt(int position) const {
//		return IWay(new WayInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_WaysGroup->ways().data()[position]));
//	}

int PrimitiveBlockInputAdaptor::waysSize() const
{
	return m_WaysCount;
}

//	IRelation PrimitiveBlockInputAdaptor::getRelationAt(int position) const {
//		return IRelation(new RelationInputAdaptor(const_cast<PrimitiveBlockInputAdaptor *>(this), m_RelationsGroup->relations().data()[position]));
//	}

int PrimitiveBlockInputAdaptor::relationsSize() const
{
	return m_RelationsCount;
}

int32_t PrimitiveBlockInputAdaptor::granularity() const
{
	return m_PrimitiveBlock->granularity();
}

int64_t PrimitiveBlockInputAdaptor::latOffset() const
{
	return m_PrimitiveBlock->lat_offset();
}

int64_t PrimitiveBlockInputAdaptor::lonOffset() const
{
	return m_PrimitiveBlock->lon_offset();
}

const std::string & PrimitiveBlockInputAdaptor::queryStringTable(int id) const
{
	return m_PrimitiveBlock->stringtable().s(id);
}

int PrimitiveBlockInputAdaptor::stringTableSize() const
{
	return m_PrimitiveBlock->stringtable().s_size();
}

int PrimitiveBlockInputAdaptor::findString(const std::string & str) const
{
	if (isNull())
		return 0;

	int size = stringTableSize();

	for (int id = 1; id < size; ++id) {
		if (str == queryStringTable(id))
			return id;
	}

	return 0;
}

INodeStream PrimitiveBlockInputAdaptor::getNodeStream()
{
	return INodeStream(this);
}

IWayStream PrimitiveBlockInputAdaptor::getWayStream()
{
	return IWayStream(this);
}

IRelationStream PrimitiveBlockInputAdaptor::getRelationStream()
{
	return IRelationStream(this);
}

} // namespace osmpbf
