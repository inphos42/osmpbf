#ifndef OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H
#define OSMPBF_PRIMITIVEBLOCKOUTPUTADAPTOR_H

#include <cstdint>
#include <string>

#include "abstractosmprimitive.h"

#include "pbfnode.h"
#include "pbfway.h"

class PrimitiveBlock;
class PrimitiveGroup;

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor {
	public:
		OSMPrimitiveBlockOutputAdaptor();
		virtual ~OSMPrimitiveBlockOutputAdaptor();

		PBFNode createNode();
		PBFWay createWay();

		bool serialize(std::string & output);
	private:
		PrimitiveBlock * m_PrimitiveBlock;
	};
}
#endif // PRIMITIVEBLOCKOUTPUTADAPTOR_H
