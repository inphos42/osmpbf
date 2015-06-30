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

#ifndef OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H

#include <osmpbf/common_input.h>
#include <osmpbf/pbf_prototypes.h>
#include <osmpbf/typelimits.h>
#include <osmpbf/dataindex.h>

#include <cstdint>
#include <string>
#include <vector>


//TODO:add id to identify a pb within a file (with which we could automatically do the filter update)

namespace osmpbf
{

class PrimitiveBlockInputAdaptor
{
/**
 * Abstracts access to a primitive block containing primitives (node, way, relations)
 * It has to be populated in synced order as OsmFileIn is not thread-safe.
 * Use one PrimitiveBlockInputAdaptor per thread as this class is not thread-safe as well
 */
public:
	PrimitiveBlockInputAdaptor();
	PrimitiveBlockInputAdaptor(char * rawData, OffsetType length, bool unpackDense = false);
	virtual ~PrimitiveBlockInputAdaptor();

	void parseData(char * rawData, OffsetType length, bool unpackDense = false);

	const std::string & queryStringTable(int id) const;
	int stringTableSize() const;
	int findString(const std::string & str) const;

//	INode getNodeAt(int position) const;
	int nodesSize(NodeTypeFlags type = PlainNode | DenseNode) const;

//	IWay getWayAt(int position) const;
	int waysSize() const;

//	IRelation getRelationAt(int position) const;
	int relationsSize() const;

	INodeStream getNodeStream();
	IWayStream getWayStream();
	IRelationStream getRelationStream();

	bool isNull() const
	{
		return !(m_PrimitiveBlock && (
			m_PlainNodesGroups.size() ||
			m_WaysGroups.size() ||
			m_DenseNodesGroups.size() ||
			m_RelationsGroups.size()));
	}

	int32_t granularity() const;

	int64_t latOffset() const;
	int64_t lonOffset() const;

	inline int64_t toWGS84Lati(int64_t rawValue) const
	{
		return (latOffset() + (granularity() * rawValue));
	}

	inline int64_t toWGS84Loni(int64_t rawValue) const
	{
		return (lonOffset() + (granularity() * rawValue));
	}

	inline double toWGS84Latd(int64_t rawValue) const
	{
		return toWGS84Lati(rawValue) * COORDINATE_SCALE_FACTOR_LAT;
	}

	inline double toWGS84Lond(int64_t rawValue) const
	{
		return toWGS84Loni(rawValue) * COORDINATE_SCALE_FACTOR_LON;
	}

private:
	friend class PlainNodeInputAdaptor;
	friend class DenseNodeInputAdaptor;
	friend class NodeStreamInputAdaptor;

	friend class WayInputAdaptor;
	friend class WayStreamInputAdaptor;

	friend class RelationInputAdaptor;
	friend class RelationStreamInputAdaptor;

	crosby::binary::PrimitiveBlock * m_PrimitiveBlock;

	PrimitiveGroupVector m_PlainNodesGroups;
	DenseNodesDataVector m_DenseNodesGroups;
	PrimitiveGroupVector m_WaysGroups;
	PrimitiveGroupVector m_RelationsGroups;

	int m_PlainNodesCount;
	int m_DenseNodesCount;
	int m_WaysCount;
	int m_RelationsCount;
};

} // namespace osmpbf

#endif // OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H
