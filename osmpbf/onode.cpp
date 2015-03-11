/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2014 Oliver Groß.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <http://www.gnu.org/licenses/>.
 */

#include "osmpbf/onode.h"

#include "osmformat.pb.h"

#include <osmpbf/primitiveblockoutputadaptor.h>

namespace osmpbf {

// ONode

	ONode::ONode(const ONode & other) : OPrimitive< NodeOutputAdaptor >(other) {}
	ONode::ONode() : OPrimitive< NodeOutputAdaptor >() {}
	ONode::ONode(NodeOutputAdaptor * data) : OPrimitive< NodeOutputAdaptor >(data) {}

	ONode & ONode::operator=(const ONode & other) { OPrimitive< NodeOutputAdaptor >::operator=(other); return *this; }

// PlainNodeOutputAdaptor

	NodeOutputAdaptor::NodeOutputAdaptor() : AbstractPrimitiveOutputAdaptor< crosby::binary::Node >() {}
	NodeOutputAdaptor::NodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, crosby::binary::Node * data) :
		AbstractPrimitiveOutputAdaptor< crosby::binary::Node >(&controller->stringTable(), data) {}

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
