#include "oway.h"

#include <queue>

#include "primitiveblockoutputadaptor.h"
#include "stringtable.h"

#include "osmformat.pb.h"

namespace osmpbf {

// OWay

	OWay::OWay(const OWay & other) : RCWrapper<WayOutputAdaptor>(other) {}
	OWay::OWay() : RCWrapper<WayOutputAdaptor>() {}
	OWay::OWay(WayOutputAdaptor * data): RCWrapper<WayOutputAdaptor>(data) {}

	OWay & OWay::operator=(const OWay & other) { RCWrapper::operator=(other); return *this; }

// WayOutputAdaptor

	WayOutputAdaptor::WayOutputAdaptor() : AbstractPrimitiveOutputAdaptor() {}
	WayOutputAdaptor::WayOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, Way * data) :
		AbstractPrimitiveOutputAdaptor(controller), m_Data(data) {}

	int64_t WayOutputAdaptor::id() const {
		return m_Data->id();
	}

	void WayOutputAdaptor::setId(int64_t value) {
		m_Data->set_id(value);
	}

	int WayOutputAdaptor::refsSize() const {
		return m_Data->refs_size();
	}

	int64_t WayOutputAdaptor::ref(int index) const {
		return m_Data->refs(index);
	}

	void WayOutputAdaptor::setRef(int index, int64_t value) {
		m_Data->set_refs(index, value);
	}

	void WayOutputAdaptor::setRefs(const DeltaFieldConstForwardIterator<int64_t> & from, const DeltaFieldConstForwardIterator<int64_t> & to) {
		clearRefs();

		for (DeltaFieldConstForwardIterator<int64_t> it = from; it != to; ++it)
			addRef(*it);
	}

	void WayOutputAdaptor::setRefs(const FieldConstIterator<int64_t> & from, const FieldConstIterator<int64_t> & to) {
		clearRefs();

		for (FieldConstIterator<int64_t> it = from; it != to; ++it)
			addRef(*it);
	}

	void WayOutputAdaptor::addRef(int64_t ref) {
		m_Data->add_refs(ref);
	}

	void WayOutputAdaptor::clearRefs() {
		m_Data->clear_refs();
	}

	int WayOutputAdaptor::tagsSize() const {
		return m_Data->keys_size();
	}

	std::string & WayOutputAdaptor::key(int index) const {
		return m_Controller->stringTable()[m_Data->keys(index)];
	}

	std::string & WayOutputAdaptor::value(int index) const {
		return m_Controller->stringTable()[m_Data->vals(index)];
	}

	void WayOutputAdaptor::addTag(const std::string & key, const std::string & value) {
		m_Data->add_keys(m_Controller->stringTable().insert(key));
		m_Data->add_vals(m_Controller->stringTable().insert(value));
	}

	void WayOutputAdaptor::removeTagLater(int index) {
		m_Controller->stringTable().remove(m_Data->keys(index));
		m_Controller->stringTable().remove(m_Data->vals(index));

		m_Data->set_keys(index, 0);
		m_Data->set_vals(index, 0);
	}

	void WayOutputAdaptor::clearTags() {
		FieldConstIterator<uint32_t> begin, end;

		begin = m_Data->keys().data();
		end = m_Data->keys().data() + m_Data->keys_size();

		for (FieldConstIterator<uint32_t> it = begin; it != end; ++it)
			m_Controller->stringTable().remove(*it);

		begin = m_Data->vals().data();
		end = m_Data->vals().data() + m_Data->vals_size();

		for (FieldConstIterator<uint32_t> it = begin; it != end; ++it)
			m_Controller->stringTable().remove(*it);

		m_Data->clear_keys();
		m_Data->clear_vals();
	}
}
