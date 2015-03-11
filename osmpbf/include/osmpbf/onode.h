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

#ifndef OSMPBF_ONODE_H
#define OSMPBF_ONODE_H

#include <osmpbf/abstractprimitiveoutputadaptor.h>
#include <osmpbf/oprimitive.h>

#include <cstdint>
#include <string>
#include <utility>

namespace crosby {
	namespace binary {
		class Node;
		class DenseNodes;
	}
}

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class NodeOutputAdaptor : public AbstractPrimitiveOutputAdaptor< crosby::binary::Node > {
	public:
		NodeOutputAdaptor();
		NodeOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, crosby::binary::Node * data);

		virtual int64_t lati() const;
		virtual void setLati(int64_t value);

		virtual int64_t loni() const;
		virtual void setLoni(int64_t value);
	};

	class ONode : public OPrimitive< NodeOutputAdaptor > {
		friend class PrimitiveBlockOutputAdaptor;
	public:
		ONode(const ONode & other);

		ONode & operator=(const ONode & other);

		inline int64_t lati() const { return m_Private->lati(); }
		inline void setLati(int64_t value) { m_Private->setLati(value); }

		inline int64_t loni() const { return m_Private->loni(); }
		inline void setLoni(int64_t value) { m_Private->setLoni(value); }

	protected:
		ONode();
		ONode(NodeOutputAdaptor * data);
	};
}

#endif // OSMPBF_ONODE_H
