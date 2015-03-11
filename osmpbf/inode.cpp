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

#include "osmpbf/inode.h"

#include "osmformat.pb.h"

#include <osmpbf/primitiveblockinputadaptor.h>

namespace osmpbf
{

// INode

INode::INode() : IPrimitive() {}
INode::INode(const INode & other) : IPrimitive(other) {}

INode & INode::operator=(const INode & other) { IPrimitive::operator=(other); return *this; }

INode::INode(AbstractNodeInputAdaptor * data) : IPrimitive(data) {}

// INodeStream

INodeStream::INodeStream() : INode() {}
INodeStream::INodeStream(const INodeStream & other) : INode(other) {}

INodeStream & INodeStream::operator=(const INodeStream & other) { INode::operator=(other); return *this; }

INodeStream::INodeStream(PrimitiveBlockInputAdaptor * controller) : INode(new NodeStreamInputAdaptor(controller)) {}

} // namespace osmpbf
