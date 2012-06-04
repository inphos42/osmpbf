#include "onode.h"

#include "primitiveblockoutputadaptor.h"
#include "stringtable.h"

#include "osmformat.pb.h"

namespace osmpbf {

// ONode

	ONode::ONode(const ONode & other) : RCWrapper<NodeOutputAdaptor>(other) {}
	ONode::ONode() : RCWrapper<NodeOutputAdaptor>() {}
	ONode::ONode(NodeOutputAdaptor * data) : RCWrapper<NodeOutputAdaptor>(data) {}

	ONode & ONode::operator=(const ONode & other) { RCWrapper::operator=(other); return *this; }

// PlainNodeOutputAdaptor

	NodeOutputAdaptor::NodeOutputAdaptor() : AbstractPrimitiveOutputAdaptor<Node>() {}
	NodeOutputAdaptor::NodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, Node * data) :
		AbstractPrimitiveOutputAdaptor<Node>(&controller->stringTable(), data) {}

	int64_t NodeOutputAdaptor::lati() const {
		return m_Data->lat();
	}

	void NodeOutputAdaptor::setLati (int64_t value) {
		m_Data->set_lat(value);
	}

	int64_t NodeOutputAdaptor::loni() const {
		return m_Data->lon();
	}

	void NodeOutputAdaptor::setLoni (int64_t value) {
		m_Data->set_lon(value);
	}
}
