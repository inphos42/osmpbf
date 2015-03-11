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

#ifndef OSMPBF_IWAY_H
#define OSMPBF_IWAY_H

#include <osmpbf/iprimitive.h>
#include <osmpbf/pbf_prototypes.h>
#include <osmpbf/common_input.h>
#include <osmpbf/wayinputadaptor.h>

#include <generics/macros.h>

#include <cstdint>
#include <string>

namespace osmpbf
{

class IWay : public IPrimitive
{
public:
	typedef osmpbf::RefIterator RefIterator;
public:
	explicit IWay(WayInputAdaptor * data);
	IWay(const IWay & other);

	IWay & operator=(const IWay & other);

	/// warning: This methods complexity is O(n). It's here for convenience. You shouldn't
	///          call this method very often or with a high index parameter.
	inline GENERICS_MARK_FUNC_DEPRECATED int64_t ref(int index) const { return static_cast< WayInputAdaptor * >(m_Private)->ref(index); }
	inline int64_t rawRef(int index) const { return static_cast< WayInputAdaptor * >(m_Private)->rawRef(index); }
	inline int refsSize() const { return static_cast< WayInputAdaptor * >(m_Private)->refsSize(); }

	inline RefIterator refBegin() const { return static_cast< WayInputAdaptor * >(m_Private)->refBegin(); }
	inline RefIterator refEnd() const { return static_cast< WayInputAdaptor * >(m_Private)->refEnd(); }

protected:
	IWay();
};

class IWayStream : public IWay
{
public:
	explicit IWayStream(PrimitiveBlockInputAdaptor * controller);
	IWayStream(const IWayStream & other);

	IWayStream & operator=(IWayStream & other);

	inline void next() { static_cast<WayStreamInputAdaptor *>(m_Private)->next(); }
	inline void previous() { static_cast<WayStreamInputAdaptor *>(m_Private)->previous(); }

protected:
	IWayStream();
	explicit IWayStream(WayInputAdaptor * data) = delete;
};

} // namespace osmpbf

#endif // OSMPBF_OSMWAY_H
