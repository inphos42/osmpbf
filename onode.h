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
		AbstractNodeOutputAdaptor() : AbstractPrimitiveOutputAdaptor() {}
		AbstractNodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, PrimitiveGroup * group, int position)
			: AbstractPrimitiveOutputAdaptor(controller, group, position) {}

		virtual int64_t lati() const = 0;
		virtual void setLati(int64_t value) = 0;

		virtual int64_t loni() const = 0;
		virtual void setLoni(int64_t value) = 0;

		virtual NodeType nodeType() const = 0;
	};

	class ONode {
	public:
		ONode();
		ONode(const ONode & other);
		ONode(PrimitiveBlockOutputAdaptor * controller, NodeType type = PlainNode);
		~virtual ONode();

		ONode & operator=(const ONode & other);

		inline int64_t id() const { m_Private->id(); }
		inline void setId(int64_t value) { m_Private->setId(value); }

		int64_t lati() const;
		void setLati(int64_t value);

		int64_t loni() const;
		void setLoni(int64_t value);

		std::pair<std::string, std::string> & tag(int index);

		void addTag(std::pair<std:string, std::string> & tag);
		void addTag(std::string key, std::string value);
		void removeTag(int index);

		int tagCount() const;
	protected:
		AbstractNodeOutputAdaptor * m_Private;
	};
}

#endif // OSMPBF_ONODE_H
