#ifndef OSMPBF_ONODE_H
#define OSMPBF_ONODE_H

#include <cstdint>
#include <string>
#include <utility>

#include "common.h"
#include "abstractprimitiveadaptor.h"

class Node;
class DenseNodes;

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class AbstractNodeOutputAdaptor : public AbstractPrimitiveOutputAdaptor {
	public:
		AbstractNodeOutputAdaptor() : AbstractPrimitiveOutputAdaptor() {}
		AbstractNodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller)
			: AbstractPrimitiveOutputAdaptor(controller) {}

		virtual int64_t lati() const = 0;
		virtual void setLati(int64_t value) = 0;

		virtual int64_t loni() const = 0;
		virtual void setLoni(int64_t value) = 0;

		virtual NodeType type() const = 0;
	};

	class ONode : public RCWrapper<AbstractNodeOutputAdaptor> {
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

		inline std::string & key(int index) { return m_Private->key(index); }
		inline std::string & value(int index) { return m_Private->value(index); }

		inline void addTag(const std::string & key, const std::string & value) { m_Private->addTag(key, value); }
		inline void removeTagLater(int index) { m_Private->removeTagLater(index); }

		inline void clearTags() { m_Private->clearTags(); }

		inline NodeType internalType() const { return m_Private->type(); }

	protected:
		ONode();
		ONode(AbstractNodeOutputAdaptor * data);
	};

	class PlainNodeOutputAdaptor : public AbstractNodeOutputAdaptor {
	public:
		PlainNodeOutputAdaptor();
		PlainNodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, Node * data);

		virtual bool isNull() const { return AbstractPrimitiveOutputAdaptor::isNULL() || !m_Data; }

		virtual int64_t id() const;
		virtual void setId(int64_t value);

		virtual int tagsSize() const;

		virtual std::string & key(int index) const;
		virtual std::string & value(int index) const;

		virtual void addTag(const std::string & key, const std::string & value);
		virtual void removeTagLater(int index);

		virtual void clearTags();

		virtual int64_t lati() const;
		virtual void setLati(int64_t value);

		virtual int64_t loni() const;
		virtual void setLoni(int64_t value);

		virtual NodeType type() const { return PlainNode; }

	protected:
		Node * m_Data;
	};

	class DenseNodeOutputAdaptor : public AbstractNodeOutputAdaptor {
	public:
		DenseNodeOutputAdaptor();
		DenseNodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, DenseNodes * data, int index);

		virtual bool isNull() const;

		virtual int64_t id() const;
		virtual void setId(int64_t value);

		virtual int tagsSize() const;

		virtual std::string & key(int index) const;
		virtual std::string & value(int index) const;

		virtual void addTag(const std::string & key, const std::string & value);
		virtual void removeTagLater(int index);

		virtual void clearTags();

		virtual int64_t lati() const;
		virtual void setLati(int64_t value);

		virtual int64_t loni() const;
		virtual void setLoni(int64_t value);

		virtual NodeType type() const { return DenseNode; }

	protected:
		DenseNodes * m_Data;
		int m_Index;
	};
}

#endif // OSMPBF_ONODE_H
