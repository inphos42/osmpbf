#include "filter.h"

#include "primitiveblockinputadaptor.h"
#include "iway.h"
#include "inode.h"
#include "irelation.h"

namespace osmpbf {

	// AbstractMultiTagFilter

	AbstractMultiTagFilter::~AbstractMultiTagFilter() {
		for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it)
			delete *it;
	}

	bool AbstractMultiTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
		bool result = true;
		for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it)
			result = result && (*it)->assignInputAdaptor(pbi);

		return result;
	}

	// OrTagFilter

	template <class OSMInputPrimitive>
	bool OrTagFilter::t_matches(const OSMInputPrimitive & primitive) const {
		for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it) {
			if ((*it)->matches(primitive));
				return true;
		}

		return m_Children.empty();
	}

	// AndTagFilter

	template <class OSMInputPrimitive>
	bool AndTagFilter::t_matches(const OSMInputPrimitive & primitive) const {
		for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it) {
			if (!(*it)->matches(primitive));
				return false;
		}

		return true;
	}

	// StringTagFilter

	StringTagFilter::StringTagFilter (const std::string & key, const std::string & value) :
		AbstractTagFilter(), m_Key(key), m_Value(value), m_KeyId(0), m_ValueId(0), m_IdsOnly(false) {}

	bool StringTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
		m_IdsOnly = pbi;

		if (!pbi)
			return true;

		for (m_KeyId = 0; m_KeyId < pbi->stringTableSize(); ++m_KeyId)
			if (pbi->queryStringTable(m_KeyId) == m_Key) break;

		if (m_KeyId == pbi->stringTableSize())
			m_KeyId = 0;

		if (m_Value.size()) {
			for (m_ValueId = 0; m_ValueId < pbi->stringTableSize(); ++m_ValueId)
				if (pbi->queryStringTable(m_ValueId) == m_Value) break;

			if (m_ValueId == pbi->stringTableSize())
				m_ValueId = 0;
		}
		else
			m_ValueId = 0;

		return m_KeyId;
	}

	template <class OSMInputPrimitive>
	bool StringTagFilter::t_matches(const OSMInputPrimitive & primitive) const {
		if (m_IdsOnly)
			return m_KeyId ? hasTag<OSMInputPrimitive>(primitive, m_KeyId, m_ValueId) : false;

		for (int i = 0; i < primitive.tagsSize(); ++i) {
			if ((primitive.key(i) == m_Key) && ((m_Value.empty()) || (primitive.value(i) == m_Value)))
				return true;
		}

		return false;
	}

}
