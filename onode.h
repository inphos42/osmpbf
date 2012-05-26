#ifndef OSMPBF_ONODE_H
#define OSMPBF_ONODE_H

#include <cstdint>
#include <string>
#include <utility>

#include "common.h"
#include "abstractprimitiveadaptor.h"

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class AbstractNodeOutputAdaptor : public AbstractPrimitiveOutputAdaptor {
	public:
		AbstractNodeOutputAdaptor() : AbstractPrimitiveOutputAdaptor() {}
		AbstractNodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, PrimitiveGroup * group, int position)
			: AbstractPrimitiveOutputAdaptor(controller, group, position) {}

		virtual int64_t lati() const = 0;
		virtual void setLati(int64_t value) = 0;

		virtual int64_t loni() const = 0;
		virtual void setLoni(int64_t value) = 0;

		virtual NodeType nodeType() const = 0;
	};

	class ONode : public RCWrapper<AbstractNodeOutputAdaptor> {
	public:
		ONode(const ONode & other);

		ONode & operator=(const ONode & other);

		inline int64_t id() const { m_Private->id(); }
		inline void setId(int64_t value) { m_Private->setId(value); }

		inline int64_t lati() const { m_Private->lati(); }
		inline void setLati(int64_t value) { m_Private->setLati(); }

		inline int64_t loni() const { m_Private->loni(); }
		inline void setLoni(int64_t value) { m_Private->setLoni(value); }

		inline std::string & key(int index) { m_Private->value(index); }
		inline std::string & value(int index) { m_Private->value(index); }

		inline void addTag(const std::string & key, const std::string & value) { m_Private->addTag(key, value); }
		inline void removeTag(int index) { m_Private->removeTag(index); }

		inline int tagCount() const;
	protected:
		ONode();
		ONode(PrimitiveBlockOutputAdaptor * controller, NodeType type = PlainNode);
	};
}

#endif // OSMPBF_ONODE_H
