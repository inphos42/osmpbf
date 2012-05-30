#include "onode.h"

#include "primitiveblockoutputadaptor.h"
#include "stringtable.h"

#include "osmformat.pb.h"

namespace osmpbf {

// ONode

	ONode::ONode(const ONode & other) : RCWrapper< osmpbf::AbstractNodeOutputAdaptor >(other) {}
	ONode::ONode() : RCWrapper<AbstractNodeOutputAdaptor>() {}
	ONode::ONode(AbstractNodeOutputAdaptor * data) : RCWrapper< osmpbf::AbstractNodeOutputAdaptor >(data) {}

	ONode & ONode::operator=(const ONode & other) { RCWrapper::operator=(other); return *this; }

// PlainNodeOutputAdaptor

	PlainNodeOutputAdaptor::PlainNodeOutputAdaptor() : AbstractNodeOutputAdaptor() {}
	PlainNodeOutputAdaptor::PlainNodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, Node * data) :
		AbstractNodeOutputAdaptor(controller), m_Data(data) {}

	int64_t PlainNodeOutputAdaptor::id() const {
		return m_Data->id();
	}

	void PlainNodeOutputAdaptor::setId(int64_t value) {
		m_Data->set_id(value);
	}

// DenseNodeOutputAdaptor

	bool DenseNodeOutputAdaptor::isNull() const {
		 return AbstractPrimitiveOutputAdaptor::isNULL() || !m_Data || (m_Index < 0) || (m_Index > m_Data->id_size() - 1) || (m_Data->mutable_id()->Get(m_Index) < 0);
	}


}
