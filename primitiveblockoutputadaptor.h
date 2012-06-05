#ifndef OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H

#include <cstdint>
#include <string>
#include <deque>

#include "common.h"

class PrimitiveBlock;
class PrimitiveGroup;

namespace osmpbf {
	class INode;
	class IWay;

	class OWay;
	class ONode;

	class StringTable;

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
		PrimitiveBlock * m_PrimitiveBlock;
		StringTable * m_StringTable;

		PrimitiveGroup * m_PlainNodesGroup;
		PrimitiveGroup * m_DenseNodesGroup;
		PrimitiveGroup * m_WaysGroup;
		PrimitiveGroup * m_RelationsGroup;

		uint32_t * prepareStringTable();
		void prepareNodes(PrimitiveGroup * nodesGroup, uint32_t * stringIdTable);
		void init();
	};
}

#endif // PRIMITIVEBLOCKOUTPUTADAPTOR_H
