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

#include <osmpbf/oway.h>
#include <osmpbf/primitiveblockoutputadaptor.h>

#include "osmformat.pb.h"

namespace osmpbf {

// OWay

	OWay::OWay(const OWay & other) : OPrimitive< WayOutputAdaptor >(other) {}
	OWay::OWay() : OPrimitive< WayOutputAdaptor >() {}
	OWay::OWay(WayOutputAdaptor * data): OPrimitive< WayOutputAdaptor >(data) {}

	OWay & OWay::operator=(const OWay & other) { OPrimitive<WayOutputAdaptor>::operator=(other); return *this; }

	void OWay::clearRefs()
	{
		m_Private->clearRefs();
	}

	// WayOutputAdaptor

	WayOutputAdaptor::WayOutputAdaptor() : AbstractPrimitiveOutputAdaptor< crosby::binary::Way >() {}
	WayOutputAdaptor::WayOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, crosby::binary::Way * data) :
		AbstractPrimitiveOutputAdaptor< crosby::binary::Way >(&controller->stringTable(), data) {}

	int WayOutputAdaptor::refsSize() const {
		return m_Data->refs_size();
	}

	int64_t WayOutputAdaptor::ref(int index) const {
		return m_Data->refs(index);
	}

	void WayOutputAdaptor::setRef(int index, int64_t value) {
		m_Data->set_refs(index, value);
	}

	void WayOutputAdaptor::setRefs(const generics::DeltaFieldConstForwardIterator<int64_t> & from, const generics::DeltaFieldConstForwardIterator<int64_t> & to) {
		if (from == to)
			return;

		clearRefs();

		for (generics::DeltaFieldConstForwardIterator<int64_t> it = from; it != to; ++it)
			addRef(*it);
	}

	void WayOutputAdaptor::setRefs(const generics::FieldConstIterator<int64_t> & from, const generics::FieldConstIterator<int64_t> & to) {
		if (from == to)
			return;

		clearRefs();

		for (generics::FieldConstIterator<int64_t> it = from; it != to; ++it)
			addRef(*it);
	}

	void WayOutputAdaptor::addRef(int64_t ref) {
		m_Data->add_refs(ref);
	}

	void WayOutputAdaptor::clearRefs() {
		m_Data->clear_refs();
	}
}
