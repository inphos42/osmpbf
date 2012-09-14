#ifndef OSMPBF_ONODE_H
#define OSMPBF_ONODE_H

#include <cstdint>
#include <string>
#include <utility>

#include "abstractprimitiveoutputadaptor.h"
#include "oprimitive.h"

namespace crosby {
namespace binary {
	class Node;
	class DenseNodes;
}
}

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class NodeOutputAdaptor : public AbstractPrimitiveOutputAdaptor< crosby::binary::Node > {
	public:
		NodeOutputAdaptor();
		NodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, crosby::binary::Node * data);

		virtual int64_t lati() const;
		virtual void setLati(int64_t value);

		virtual int64_t loni() const;
		virtual void setLoni(int64_t value);
	};

	class ONode : public OPrimitive< NodeOutputAdaptor > {
		friend class PrimitiveBlockOutputAdaptor;
	public:
		ONode(const ONode & other);

		ONode & operator=(const ONode & other);

		inline int64_t lati() const { return m_Private->lati(); }
		inline void setLati(int64_t value) { m_Private->setLati(value); }

		inline int64_t loni() const { return m_Private->loni(); }
		inline void setLoni(int64_t value) { m_Private->setLoni(value); }

// 		inline NodeType internalType() const { return m_Private->type(); }

	protected:
		ONode();
		ONode(NodeOutputAdaptor * data);
	};
}

#endif // OSMPBF_ONODE_H
