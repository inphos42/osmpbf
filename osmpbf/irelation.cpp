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

#include <osmpbf/irelation.h>
#include <osmpbf/common.h>
#include <osmpbf/primitiveblockinputadaptor.h>

#include "osmformat.pb.h"

namespace osmpbf {

// IRelation

	IRelation::IRelation() : IPrimitive() {}
	IRelation::IRelation(RelationInputAdaptor * data) : IPrimitive(data) {}
	IRelation::IRelation(const IRelation & other) : IPrimitive(other) {}

// IRelationStream

	IRelationStream::IRelationStream(PrimitiveBlockInputAdaptor * controller) : IRelation(new RelationStreamInputAdaptor(controller)) {}
	IRelationStream::IRelationStream(const IRelationStream & other) : IRelation(other) {}

// IMemberStream

	IMemberStream::IMemberStream(const IMemberStream & other) : RCWrapper<MemberStreamInputAdaptor>(other) {}
	IMemberStream::IMemberStream(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Relation * data)
		: RCWrapper<MemberStreamInputAdaptor>(new MemberStreamInputAdaptor(data)), m_Controller(controller) {}

	const std::string & IMemberStream::role() const {
		return m_Controller->queryStringTable(m_Private->roleId());
	}

// RelationInputAdaptor

	RelationInputAdaptor::RelationInputAdaptor() : AbstractPrimitiveInputAdaptor(), m_Data(NULL) {}
	RelationInputAdaptor::RelationInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Relation * data)
		: AbstractPrimitiveInputAdaptor(controller), m_Data(data) {}

	int64_t RelationInputAdaptor::id() {
		return m_Data->id();
	}

	PrimitiveType RelationInputAdaptor::type() const {
		return PrimitiveType::RelationPrimitive;
	}


	int RelationInputAdaptor::tagsSize() const {
		return m_Data->keys_size();
	}

	uint32_t RelationInputAdaptor::keyId(int index) const {
		return m_Data->keys(index);
	}

	uint32_t RelationInputAdaptor::valueId(int index) const {
		return m_Data->vals(index);
	}

	int RelationInputAdaptor::membersSize() const {
		return m_Data->memids_size();
	}

// RelationStreamInputAdaptor

	RelationStreamInputAdaptor::RelationStreamInputAdaptor() : RelationInputAdaptor(), m_Index(0), m_MaxIndex(0) {}
	RelationStreamInputAdaptor::RelationStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller)
		: RelationInputAdaptor(controller, controller->m_RelationsGroup ? controller->m_RelationsGroup->relations().data()[0] : NULL), m_Index(0), m_MaxIndex(m_Controller->relationsSize()) {}

	bool RelationStreamInputAdaptor::isNull() const {
		return RelationInputAdaptor::isNull() || (m_Index >= m_MaxIndex) || (m_Index < 0);
	}

	void RelationStreamInputAdaptor::next() {
		m_Index++;
		if (m_Index < m_MaxIndex) {
			m_Data = m_Controller->m_RelationsGroup->relations().data()[m_Index];
		}
		else {
			m_Data = 0;
		}
	}

	void RelationStreamInputAdaptor::previous() {
		m_Index--;
		if (m_Index >= 0 && m_Index < m_MaxIndex) {
			m_Data = m_Controller->m_RelationsGroup->relations().data()[m_Index];
		}
		else {
			m_Data = 0;
		}
	}

// MemberStreamInputAdaptor

	MemberStreamInputAdaptor::MemberStreamInputAdaptor() : m_Data(NULL), m_Index(0), m_MaxIndex(0), m_CachedId(0) {}
	MemberStreamInputAdaptor::MemberStreamInputAdaptor(const crosby::binary::Relation * data) : m_Data(data), m_Index(0),
		m_MaxIndex(data ? data->memids_size() : 0), m_CachedId(data && m_MaxIndex > 0 ?  data->memids(0) : 0) {}

	PrimitiveType MemberStreamInputAdaptor::type() const {
		return PrimitiveType(m_Data->types(m_Index));
	}

	uint32_t MemberStreamInputAdaptor::roleId() const {
		return m_Data->roles_sid(m_Index);
	}

	void MemberStreamInputAdaptor::next() {
		m_Index++;

		if (isNull())
			return;

		m_CachedId += m_Data->memids(m_Index);
	}

	void MemberStreamInputAdaptor::previous() {
		m_Index--;

		if (isNull())
			return;

		m_CachedId -= m_Data->memids(m_Index + 1);
	}
}
