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

#include <osmpbf/iway.h>
#include <osmpbf/primitiveblockinputadaptor.h>

#include "osmformat.pb.h"

namespace osmpbf {

// IWay

	IWay::IWay() : IPrimitive() {};
	IWay::IWay(const IWay & other) : IPrimitive(other) {}
	IWay::IWay(WayInputAdaptor * data) : IPrimitive(data) {}

// IWayStream

	IWayStream::IWayStream() : IWay() {}
	IWayStream::IWayStream(const IWayStream & other) : IWay(other) {}
	IWayStream::IWayStream(PrimitiveBlockInputAdaptor * controller) : IWay(new WayStreamInputAdaptor(controller)) {}

// WayInputAdaptor

	WayInputAdaptor::WayInputAdaptor() : AbstractPrimitiveInputAdaptor(), m_Data(NULL) {}
	WayInputAdaptor::WayInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Way * data)
		: AbstractPrimitiveInputAdaptor(controller), m_Data(data) {}

	int64_t WayInputAdaptor::id() {
		return m_Data->id();
	}

	PrimitiveType WayInputAdaptor::type() const {
		return osmpbf::PrimitiveType::WayPrimitive;
	}


	int64_t WayInputAdaptor::ref(int index) const {
		int64_t result = m_Data->refs(0);
		for (int i = 1; i <= index; i++)
			result += m_Data->refs(i);

		return result;
	}

	int WayInputAdaptor::refsSize() const {
		return m_Data->refs_size();
	}

	int64_t WayInputAdaptor::rawRef(int index) const {
		return m_Data->refs(index);
	}

	WayInputAdaptor::RefIterator WayInputAdaptor::refBegin() const {
		return WayInputAdaptor::RefIterator(m_Data->refs().data());
	}

	WayInputAdaptor::RefIterator WayInputAdaptor::refEnd() const {
		return WayInputAdaptor::RefIterator(m_Data->refs().data() + m_Data->refs_size());
	}

	int WayInputAdaptor::tagsSize() const {
		return m_Data->keys_size();
	}

	uint32_t WayInputAdaptor::keyId(int index) const {
		return m_Data->keys(index);
	}

	uint32_t WayInputAdaptor::valueId(int index) const {
		return m_Data->vals(index);
	}

// WayStreamInputAdaptor

	WayStreamInputAdaptor::WayStreamInputAdaptor() : WayInputAdaptor(), m_Index(0), m_MaxIndex(0) {}
	WayStreamInputAdaptor::WayStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller)
		: WayInputAdaptor(controller, controller->m_WaysGroup ? controller->m_WaysGroup->ways().data()[0] : NULL), m_Index(0), m_MaxIndex(m_Controller->waysSize()) {}

	bool WayStreamInputAdaptor::isNull() const {
		return AbstractPrimitiveInputAdaptor::isNull() || (m_Index >= m_MaxIndex) || (m_Index < 0);
	}

	void WayStreamInputAdaptor::next() {
		m_Index++;
		m_Data = m_Controller->m_WaysGroup->ways().data()[m_Index];
	}

	void WayStreamInputAdaptor::previous() {
		m_Index--;
		m_Data = m_Controller->m_WaysGroup->ways().data()[m_Index];
	}
}
