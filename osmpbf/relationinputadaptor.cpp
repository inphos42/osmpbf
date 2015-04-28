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

#include <osmpbf/relationinputadaptor.h>
#include <osmpbf/primitiveblockinputadaptor.h>

#include "osmformat.pb.h"

namespace osmpbf
{

// IMemberStream

IMemberStream::IMemberStream(const IMemberStream & other) : RCWrapper<MemberStreamInputAdaptor>(other) {}

IMemberStream &IMemberStream::operator=(const IMemberStream & other) {
	RCWrapper<MemberStreamInputAdaptor>::operator=(other);
	m_Controller = other.m_Controller;
	return *this;
}

bool IMemberStream::isNull() const
{
	return !m_Controller || RCWrapper< MemberStreamInputAdaptor >::isNull() || RCWrapper< MemberStreamInputAdaptor >::m_Private->isNull();
}

IMemberStream::IMemberStream(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Relation * data)
	: RCWrapper<MemberStreamInputAdaptor>(new MemberStreamInputAdaptor(data)), m_Controller(controller) {}

const std::string & IMemberStream::role() const
{
	return m_Controller->queryStringTable(m_Private->roleId());
}

// RelationInputAdaptor

RelationInputAdaptor::RelationInputAdaptor() : AbstractPrimitiveInputAdaptor(), m_Data(NULL) {}
RelationInputAdaptor::RelationInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Relation * data)
: AbstractPrimitiveInputAdaptor(controller), m_Data(data) {}

bool RelationInputAdaptor::isNull() const
{
	return AbstractPrimitiveInputAdaptor::isNull() || !m_Data;
}

int64_t RelationInputAdaptor::id()
{
	return m_Data->id();
}

PrimitiveType RelationInputAdaptor::type() const
{
	return PrimitiveType::RelationPrimitive;
}

int RelationInputAdaptor::tagsSize() const
{
	return m_Data->keys_size();
}

uint32_t RelationInputAdaptor::keyId(int index) const
{
	return m_Data->keys(index);
}

uint32_t RelationInputAdaptor::valueId(int index) const
{
	return m_Data->vals(index);
}

bool RelationInputAdaptor::hasInfo() const {
	return m_Data->has_info();
}

IInfo RelationInputAdaptor::info() const {
	if (!hasInfo())
	{
		return IInfo();
	}
	return IInfo(m_Data->info());
}

int RelationInputAdaptor::membersSize() const
{
	return m_Data->memids_size();
}

IMemberStream RelationInputAdaptor::getMemberStream() const
{
	return IMemberStream(m_Controller, m_Data);
}

// RelationStreamInputAdaptor

RelationStreamInputAdaptor::RelationStreamInputAdaptor() : RelationInputAdaptor(), m_Index(0) {}
RelationStreamInputAdaptor::RelationStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller)
	: RelationInputAdaptor(controller, nullptr),
	  m_Index(-1),
	  m_GroupIterator(m_Controller->m_RelationsGroups.begin())
{
	next();
}

bool RelationStreamInputAdaptor::isNull() const
{
	return RelationInputAdaptor::isNull() || (m_Index < 0) || (getCurrentGroup() == nullptr);
}

void RelationStreamInputAdaptor::next()
{
	crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (group)
	{
		m_Index++;

		if (m_Index >= group->relations_size())
		{
			m_GroupIterator++;
			m_Index = 0;

			group = getCurrentGroup();
		}
	}

	m_Data = group ? group->relations().data()[m_Index] : nullptr;
}

void RelationStreamInputAdaptor::previous()
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
				m_Index = group ? group->relations_size() - 1 : -1;
			}
		}
	}

	m_Data = group ? group->relations().data()[m_Index] : nullptr;
}

crosby::binary::PrimitiveGroup * RelationStreamInputAdaptor::getCurrentGroup() const
{
	return m_GroupIterator == m_Controller->m_RelationsGroups.end() ? nullptr : *m_GroupIterator;
}

// MemberStreamInputAdaptor

MemberStreamInputAdaptor::MemberStreamInputAdaptor() : m_Data(NULL), m_Index(0), m_MaxIndex(0), m_CachedId(0) {}
MemberStreamInputAdaptor::MemberStreamInputAdaptor(const crosby::binary::Relation * data) : m_Data(data), m_Index(0),
	m_MaxIndex(data ? data->memids_size() : 0), m_CachedId(data && m_MaxIndex > 0 ?  data->memids(0) : 0) {}

bool MemberStreamInputAdaptor::isNull() const { return !m_Data || m_Index >= m_MaxIndex || m_Index < 0; }

int64_t MemberStreamInputAdaptor::id() const
{
	return m_CachedId;
}

PrimitiveType MemberStreamInputAdaptor::type() const
{
	crosby::binary::Relation_MemberType pbfMemType = m_Data->types(m_Index);
	switch (pbfMemType)
	{
	case (crosby::binary::Relation_MemberType::Relation_MemberType_NODE):
		return PrimitiveType::NodePrimitive;
	case (crosby::binary::Relation_MemberType::Relation_MemberType_WAY):
		return PrimitiveType::WayPrimitive;
	case (crosby::binary::Relation_MemberType::Relation_MemberType_RELATION):
		return PrimitiveType::RelationPrimitive;
	default:
		return PrimitiveType::InvalidPrimitive;
	}
}

uint32_t MemberStreamInputAdaptor::roleId() const
{
	return m_Data->roles_sid(m_Index);
}

void MemberStreamInputAdaptor::next()
{
	m_Index++;

	if (isNull())
		return;

	m_CachedId += m_Data->memids(m_Index);
}

void MemberStreamInputAdaptor::previous()
{
	m_Index--;

	if (isNull())
		return;

	m_CachedId -= m_Data->memids(m_Index + 1);
}

} // namespace osmpbf
