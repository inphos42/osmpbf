#include "oway.h"

#include "primitiveblockoutputadaptor.h"

#include "osmformat.pb.h"

namespace osmpbf {

// OWay

	OWay::OWay(const OWay & other) : OPrimitive< WayOutputAdaptor >(other) {}
	OWay::OWay() : OPrimitive< WayOutputAdaptor >() {}
	OWay::OWay(WayOutputAdaptor * data): OPrimitive< WayOutputAdaptor >(data) {}

	OWay & OWay::operator=(const OWay & other) { OPrimitive<WayOutputAdaptor>::operator=(other); return *this; }

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
