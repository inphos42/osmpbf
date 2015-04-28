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

#include <osmpbf/wayinputadaptor.h>
#include <osmpbf/primitiveblockinputadaptor.h>

#include "osmformat.pb.h"

namespace osmpbf
{

// WayInputAdaptor

WayInputAdaptor::WayInputAdaptor() : AbstractPrimitiveInputAdaptor(), m_Data(nullptr) {}
WayInputAdaptor::WayInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Way * data)
	: AbstractPrimitiveInputAdaptor(controller), m_Data(data) {}

bool WayInputAdaptor::isNull() const
{
	return AbstractPrimitiveInputAdaptor::isNull() || !m_Data;
}

int64_t WayInputAdaptor::id()
{
	return m_Data->id();
}

PrimitiveType WayInputAdaptor::type() const
{
	return osmpbf::PrimitiveType::WayPrimitive;
}

int64_t WayInputAdaptor::ref(int index) const
{
	int64_t result = m_Data->refs(0);
	for (int i = 1; i <= index; i++)
		result += m_Data->refs(i);

	return result;
}

int WayInputAdaptor::refsSize() const
{
	return m_Data->refs_size();
}

int64_t WayInputAdaptor::rawRef(int index) const
{
	return m_Data->refs(index);
}

RefIterator WayInputAdaptor::refBegin() const
{
	return RefIterator(m_Data->refs().data());
}

RefIterator WayInputAdaptor::refEnd() const
{
	return RefIterator(m_Data->refs().data() + m_Data->refs_size());
}

int WayInputAdaptor::tagsSize() const
{
	return m_Data->keys_size();
}

uint32_t WayInputAdaptor::keyId(int index) const
{
	return m_Data->keys(index);
}

uint32_t WayInputAdaptor::valueId(int index) const
{
	return m_Data->vals(index);
}

bool WayInputAdaptor::hasInfo() const {
	return m_Data->has_info();
}

IInfo WayInputAdaptor::info() const {
	if (hasInfo())
	{
		return IInfo();
	}
	return IInfo(m_Data->info());
}

// WayStreamInputAdaptor

WayStreamInputAdaptor::WayStreamInputAdaptor() : WayInputAdaptor(), m_Index(-1) {}
WayStreamInputAdaptor::WayStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller)
	: WayInputAdaptor(controller, nullptr),
	  m_Index(-1),
	  m_GroupIterator(m_Controller->m_WaysGroups.begin())
{
	next();
}

bool WayStreamInputAdaptor::isNull() const
{
	return WayInputAdaptor::isNull() || (m_Index < 0) || (getCurrentGroup() == nullptr);
}

void WayStreamInputAdaptor::next()
{
	crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (group)
	{
		m_Index++;

		if (m_Index >= group->ways_size())
		{
			m_GroupIterator++;
			m_Index = 0;

			group = getCurrentGroup();
		}
	}

	m_Data = group ? group->ways().data()[m_Index] : nullptr;
}

void WayStreamInputAdaptor::previous()
{
	crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (group)
	{
		m_Index--;

		if (m_Index < 0)
		{
			if (m_GroupIterator == m_Controller->m_RelationsGroups.begin())
			{
				group = nullptr;
			}
			else
			{
				m_GroupIterator--;

				group = getCurrentGroup();
				m_Index = group ? group->ways_size() - 1 : -1;
			}
		}
	}

	m_Data = group ? group->ways().data()[m_Index] : nullptr;
}

crosby::binary::PrimitiveGroup * WayStreamInputAdaptor::getCurrentGroup() const
{
	return m_GroupIterator == m_Controller->m_WaysGroups.end() ? nullptr : *m_GroupIterator;
}

} // namespace osmpbf
