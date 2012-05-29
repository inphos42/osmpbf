#ifndef OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H

#include <cstdint>
#include <string>
#include <set>

// #include "onode.h"
#include "oway.h"

// #include "stringtable.h"

class PrimitiveBlock;
class PrimitiveGroup;

namespace osmpbf {
	class IWay;
	class StringTable;

	class PrimitiveBlockOutputAdaptor {
	public:
		PrimitiveBlockOutputAdaptor();
		virtual ~PrimitiveBlockOutputAdaptor();

// 		ONode createNode();
// 		ONode createNode(INode & templateINode);

		int nodesSize();

		OWay createWay();
		OWay createWay(const IWay & templateIWay);

		int waysSize();

		void setGranularity(int32_t value);
		void setLatOffset(int64_t value);
		void setLonOffset(int64_t value);

		inline StringTable & stringTable() { return *m_StringTable; }

// 		bool isInitialized() const;

		bool flush(std::string & output);

// 		PrimitiveBlockOutputAdaptor & operator<<(INode & node) { createNode(node); return *this; }
		inline PrimitiveBlockOutputAdaptor & operator<<(IWay & way) { createWay(way); return *this; }

	private:
		PrimitiveBlock * m_PrimitiveBlock;
		StringTable * m_StringTable;

		PrimitiveGroup * m_NodesGroup;
		PrimitiveGroup * m_DenseNodesGroup;
		PrimitiveGroup * m_WaysGroup;
		PrimitiveGroup * m_RelationsGroup;
	};
}

#endif // PRIMITIVEBLOCKOUTPUTADAPTOR_H
