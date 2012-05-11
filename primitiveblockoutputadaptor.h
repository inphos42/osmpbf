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
		OSMPrimitiveBlockOutputAdaptor();
		virtual ~OSMPrimitiveBlockOutputAdaptor();

		ONode createNode();
		ONode createNode(INode & templateNode);

		OWay createWay();
		OWay createWay(IWay & templateWay);

		PrimitiveBlockOutputAdaptor & operator<<(INode & node);
		PrimitiveBlockOutputAdaptor & operator<<(IWay & way);

		bool serialize(std::string & output);
	private:
		PrimitiveBlock * m_PrimitiveBlock;
	};
}

#endif // PRIMITIVEBLOCKOUTPUTADAPTOR_H
