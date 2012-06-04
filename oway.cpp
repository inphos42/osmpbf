#include "oway.h"

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
		AbstractPrimitiveOutputAdaptor(&controller->stringTable(), data) {}

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
		if (from == to)
			return;

		clearRefs();

		for (DeltaFieldConstForwardIterator<int64_t> it = from; it != to; ++it)
			addRef(*it);
	}

	void WayOutputAdaptor::setRefs(const FieldConstIterator<int64_t> & from, const FieldConstIterator<int64_t> & to) {
		if (from == to)
			return;

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
}
