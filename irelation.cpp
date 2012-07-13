#include "irelation.h"

#include "primitiveblockinputadaptor.h"
#include "osmformat.pb.h"

namespace osmpbf {

// IRelation

	IRelation::IRelation() : RCWrapper<RelationInputAdaptor>() {}
	IRelation::IRelation(RelationInputAdaptor * data) : RCWrapper<RelationInputAdaptor>(data) {}
	IRelation::IRelation(const IRelation & other) : RCWrapper<RelationInputAdaptor>(other) {}

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

	RelationInputAdaptor::RelationInputAdaptor() : AbstractPrimitiveInputAdaptor() {}
	RelationInputAdaptor::RelationInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Relation * data)
		: AbstractPrimitiveInputAdaptor(controller) {}

	int64_t RelationInputAdaptor::id() {
		return m_Data->id();
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

	RelationStreamInputAdaptor::RelationStreamInputAdaptor() : RelationInputAdaptor(), m_Index(-1), m_MaxIndex(0) {}
	RelationStreamInputAdaptor::RelationStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller)
		: RelationInputAdaptor(controller, controller->m_RelationsGroup ? controller->m_RelationsGroup->relations().data()[0] : NULL), m_MaxIndex(m_Controller->relationsSize()) {}

	bool RelationStreamInputAdaptor::isNull() const {
		return osmpbf::RelationInputAdaptor::isNull() || m_Index >= m_MaxIndex || m_Index < 0;
	}

	void RelationStreamInputAdaptor::next() {
		m_Index++;
		m_Data = m_Controller->m_RelationsGroup->relations().data()[m_Index];
	}

	void RelationStreamInputAdaptor::previous() {
		m_Index--;
		m_Data = m_Controller->m_RelationsGroup->relations().data()[m_Index];
	}

// MemberStreamInputAdaptor

	MemberStreamInputAdaptor::MemberStreamInputAdaptor() : m_Data(NULL), m_Index(0), m_MaxIndex(0), m_CachedId(0) {}

	MemberStreamInputAdaptor::MemberStreamInputAdaptor(const crosby::binary::Relation * data) : m_Data(data), m_Index(0), m_MaxIndex(data ? data->memids_size() : 0), m_CachedId(0) {}

	PrimitiveType MemberStreamInputAdaptor::type() const {
		return PrimitiveType(m_Data->types(m_Index));
	}

	uint32_t MemberStreamInputAdaptor::roleId() const {
		return m_Data->roles_sid(m_Index);
	}

	void MemberStreamInputAdaptor::next() {
		m_Index++;
		m_CachedId += m_Data->memids(m_Index);
	}

	void MemberStreamInputAdaptor::previous() {
		m_CachedId -= m_Data->memids(m_Index);
		m_Index--;
	}
}
