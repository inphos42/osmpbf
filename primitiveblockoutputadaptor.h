#ifndef OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H

#include <cstdint>
#include <string>

#include "onode.h"
#include "oway.h"

#include "inode.h"
#include "iway.h"

class PrimitiveBlock;
class PrimitiveGroup;

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor {
	public:
		PrimitiveBlockOutputAdaptor();
		virtual ~PrimitiveBlockOutputAdaptor();

		bool isInitialized() const;

		ONode createNode();
		ONode createNode(INode & templateNode);

		int nodesSize();

		OWay createWay();
		OWay createWay(IWay & templateWay);

		int waysSize();

		void setGranularity(int32_t value);
		void setLatOffset(int64_t value);
		void setLonOffset(int64_t value);

		PrimitiveBlockOutputAdaptor & operator<<(INode & node);
		PrimitiveBlockOutputAdaptor & operator<<(IWay & way);

		bool serialize(std::string & output);
	private:
		PrimitiveBlock * m_PrimitiveBlock;

		void buildStringTable();
	};
}

#endif // PRIMITIVEBLOCKOUTPUTADAPTOR_H
