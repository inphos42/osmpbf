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

#include <cstdint>
#include <string>

#include <osmpbf/common.h>
#include <osmpbf/typelimits.h>

namespace crosby {
	namespace binary {
		class PrimitiveBlock;
		class PrimitiveGroup;
	}
}

namespace osmpbf {
	class INode;
	class IWay;
	class IRelation;

	class INodeStream;
	class IWayStream;
	class IRelationStream;

	class PrimitiveBlockInputAdaptor {
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

		INode getNodeAt(int position) const;
		int nodesSize(unsigned char type = PlainNode | DenseNode) const;

		IWay getWayAt(int position) const;
		int waysSize() const;

		IRelation getRelationAt(int position) const;
		int relationsSize() const;

		INodeStream getNodeStream();
		IWayStream getWayStream();
		IRelationStream getRelationStream();

		bool isNull() const {
			return !(m_PrimitiveBlock && (
				m_PlainNodesGroup ||
				m_WaysGroup ||
				m_DenseNodesGroup ||
				m_RelationsGroup));
		}

		int32_t granularity() const;

		int64_t latOffset() const;
		int64_t lonOffset() const;

		inline int64_t toWGS84Lati(int64_t rawValue) const {
			return (latOffset() + (granularity() * rawValue));
		}

		inline int64_t toWGS84Loni(int64_t rawValue) const {
			return (lonOffset() + (granularity() * rawValue));
		}

		inline double toWGS84Latd(int64_t rawValue) const {
			return toWGS84Lati(rawValue) * .000000001;
		}

		inline double toWGS84Lond(int64_t rawValue) const {
			return toWGS84Loni(rawValue) * .000000001;
		}

		void unpackDenseNodes();
		inline bool denseNodesUnpacked() const { return m_DenseNodesUnpacked; }
	private:
		friend class PlainNodeInputAdaptor;
		friend class DenseNodeInputAdaptor;
		friend class NodeStreamInputAdaptor;

		friend class WayInputAdaptor;
		friend class WayStreamInputAdaptor;

		friend class RelationInputAdaptor;
		friend class RelationStreamInputAdaptor;

		crosby::binary::PrimitiveBlock * m_PrimitiveBlock;

		crosby::binary::PrimitiveGroup * m_PlainNodesGroup;
		crosby::binary::PrimitiveGroup * m_DenseNodesGroup;
		crosby::binary::PrimitiveGroup * m_WaysGroup;
		crosby::binary::PrimitiveGroup * m_RelationsGroup;

		bool m_DenseNodesUnpacked;
		int * m_DenseNodeKeyValIndex;

		inline int queryDenseNodeKeyValIndex(int index) {
			if (!m_DenseNodeKeyValIndex) buildDenseNodeKeyValIndex();
			return m_DenseNodeKeyValIndex[index];
		}

		void buildDenseNodeKeyValIndex();
	};
}

#endif // OSMPBF_PRIMITIVEBLOCKINPUTADAPTOR_H
