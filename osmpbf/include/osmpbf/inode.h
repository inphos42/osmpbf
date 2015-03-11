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

#ifndef OSMPBF_INNODE_H
#define OSMPBF_INNODE_H

#include <osmpbf/common_input.h>
#include <osmpbf/iprimitive.h>
#include <osmpbf/abstractnodeinputadaptor.h>
#include <osmpbf/nodestreaminputadaptor.h>

#include <cstdint>
#include <string>

namespace osmpbf
{

class INode : public IPrimitive
{
public:
	explicit INode(AbstractNodeInputAdaptor * data);
	INode(const INode & other);

	INode & operator=(const INode & other);

	inline int64_t lati() const { return static_cast< AbstractNodeInputAdaptor * >(m_Private)->lati(); }
	inline int64_t loni() const { return static_cast< AbstractNodeInputAdaptor * >(m_Private)->loni(); }

	inline double latd() const { return static_cast< AbstractNodeInputAdaptor * >(m_Private)->latd(); }
	inline double lond() const { return static_cast< AbstractNodeInputAdaptor * >(m_Private)->lond(); }

	inline int64_t rawLat() const { return static_cast< AbstractNodeInputAdaptor * >(m_Private)->rawLat(); }
	inline int64_t rawLon() const { return static_cast< AbstractNodeInputAdaptor * >(m_Private)->rawLon(); }

	inline NodeType internalNodeType() const { return static_cast< AbstractNodeInputAdaptor * >(m_Private)->nodeType(); }

protected:
	INode();
};

class INodeStream : public INode
{
public:
	explicit INodeStream(PrimitiveBlockInputAdaptor * controller);
	INodeStream(const INodeStream & other);

	INodeStream & operator=(const INodeStream & other);

	inline void next() { static_cast< NodeStreamInputAdaptor * >(m_Private)->next(); }
	inline void previous() { static_cast< NodeStreamInputAdaptor * >(m_Private)->previous(); }

protected:
	INodeStream();
	INodeStream(AbstractNodeInputAdaptor * data);
};

} // namespace osmpbf

#endif // OSMPBF_INNODE_H
