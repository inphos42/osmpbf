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

#ifndef OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H

#include <osmpbf/common.h>
#include <osmpbf/stringtable_fwd.h>

#include <cstdint>
#include <string>

namespace crosby {
	namespace binary {
		class PrimitiveBlock;
		class PrimitiveGroup;
	}
}

namespace osmpbf {
	class INode;
	class IWay;

	class OWay;
	class ONode;

	class PrimitiveBlockOutputAdaptor {
	public:
		PrimitiveBlockOutputAdaptor();
		virtual ~PrimitiveBlockOutputAdaptor();

		ONode createNode(NodeType type);
		ONode createNode(INode & templateINode);
		ONode createNode(INode & templateINode, NodeType type);

		int nodesSize(NodeType type) const;

		OWay createWay();
		OWay createWay(const IWay & templateIWay);

		int waysSize() const;

		void setGranularity(int32_t value);
		void setLatOffset(int64_t value);
		void setLonOffset(int64_t value);

		inline StringTable & stringTable() { return *m_StringTable; }

		bool flush(std::string & buffer);

		PrimitiveBlockOutputAdaptor & operator<<(INode & node);
		PrimitiveBlockOutputAdaptor & operator<<(IWay & way);

	private:
		crosby::binary::PrimitiveBlock * m_PrimitiveBlock;
		StringTable * m_StringTable;

		crosby::binary::PrimitiveGroup * m_PlainNodesGroup;
		crosby::binary::PrimitiveGroup * m_DenseNodesGroup;
		crosby::binary::PrimitiveGroup * m_WaysGroup;
		crosby::binary::PrimitiveGroup * m_RelationsGroup;

		uint32_t * prepareStringTable();
		void prepareNodes(crosby::binary::PrimitiveGroup * nodesGroup, uint32_t * stringIdTable);
		void init();
	};
}

#endif // PRIMITIVEBLOCKOUTPUTADAPTOR_H
