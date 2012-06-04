#ifndef OSMPBF_ONODE_H
#define OSMPBF_ONODE_H

#include <cstdint>
#include <string>
#include <utility>

#include "abstractprimitiveoutputadaptor.h"

class Node;
class DenseNodes;

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class NodeOutputAdaptor : public AbstractPrimitiveOutputAdaptor<Node> {
	public:
		NodeOutputAdaptor();
		NodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, Node * data);

		virtual int64_t lati() const;
		virtual void setLati(int64_t value);

		virtual int64_t loni() const;
		virtual void setLoni(int64_t value);
	};

	class ONode : public RCWrapper<NodeOutputAdaptor> {
		friend class PrimitiveBlockOutputAdaptor;
	public:
		ONode(const ONode & other);

		ONode & operator=(const ONode & other);

		inline int64_t id() const { return m_Private->id(); }
		inline void setId(int64_t value) { m_Private->setId(value); }

		inline int64_t lati() const { return m_Private->lati(); }
		inline void setLati(int64_t value) { m_Private->setLati(value); }

		inline int64_t loni() const { return m_Private->loni(); }
		inline void setLoni(int64_t value) { m_Private->setLoni(value); }

		inline int tagsSize() const { return m_Private->tagsSize(); }

		inline const std::string & key(int index) { return m_Private->key(index); }
		inline const std::string & value(int index) { return m_Private->value(index); }

		inline void addTag(const std::string & key, const std::string & value) { m_Private->addTag(key, value); }
		inline void removeTagLater(int index) { m_Private->removeTagLater(index); }

		inline void clearTags() { m_Private->clearTags(); }

// 		inline NodeType internalType() const { return m_Private->type(); }

	protected:
		ONode();
		ONode(NodeOutputAdaptor * data);
	};
}

#endif // OSMPBF_ONODE_H
