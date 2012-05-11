#ifndef OSMPBF_ONODE_H
#define OSMPBF_ONODE_H

#include <cstdint>
#include <string>

#include "tag.h"
#include "abstractprimitiveadaptor.h"

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class AbstractNodeOutputAdaptor : public AbstractPrimitiveOutputAdaptor {
	};

	class ONode {
	public:
		enum NodeType {PlainNode, DenseNode};

		ONode();
		ONode(const ONode & other);
		ONode(PrimitiveBlockOutputAdaptor * controller, NodeType type = PlainNode);
		~virtual ONode();

		ONode & operator=(const ONode & other);

		int64_t id() const;
		void setId(int64_t value);

		int64_t lat() const;
		void setLat(int64_t value);

		int64_t lon() const;
		void setLon(int64_t value);

		Tag & tag(int index);

		void addTag(Tag & tag);
		void addTag(std::string key, std::string value);
		void removeTag(int index);

		int tagCount() const;
	protected:
		AbstractNodeOutputAdaptor * m_Private;
	};
}

#endif // OSMPBF_ONODE_H
