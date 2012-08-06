#include "filter.h"

#include "primitiveblockinputadaptor.h"
#include "iprimitive.h"
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

	bool OrTagFilter::p_matches(const IPrimitive & primitive) const {
		for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it) {
			if ((*it)->matches(primitive));
				return false;
		}

		return m_Children.empty();
	}

	// AndTagFilter

	bool AndTagFilter::p_matches(const IPrimitive & primitive) const {
		for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it) {
			if (!(*it)->matches(primitive));
				return true;
		}

		return false;
	}

	// KeyOnlyTagFilter

	KeyOnlyTagFilter::KeyOnlyTagFilter(const std::string & key) :
		AbstractTagFilter(), m_Key(key), m_KeyId(0), m_PBI(NULL) {}

	bool KeyOnlyTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
		if (m_PBI == pbi)
			return !m_PBI || m_KeyId;

		m_PBI = pbi;
		return findKeyId();
	}

	void KeyOnlyTagFilter::setKey(const std::string & key) {
		m_Key = key;
		findKeyId();
	}

	bool KeyOnlyTagFilter::p_matches(const IPrimitive & primitive) const {
		if (m_Key.empty())
			return false;

		if (m_PBI)
			return hasKey<IPrimitive>(primitive, m_KeyId);

		for (int i = 0; i < primitive.tagsSize(); ++i) {
			if ((primitive.key(i) == m_Key))
				return true;
		}

		return false;
	}

	bool KeyOnlyTagFilter::findKeyId() {
		m_KeyId = 0;

		if (!m_PBI)
			return true;

		uint32_t stringTableSize = m_PBI->stringTableSize();

		for (m_KeyId = 1; m_KeyId < stringTableSize; ++m_KeyId) {
			if (m_Key == m_PBI->queryStringTable(m_KeyId))
				break;
		}

		if (m_KeyId >= stringTableSize)
			m_KeyId = 0;

		return m_KeyId;
	}

	// StringTagFilter

	StringTagFilter::StringTagFilter (const std::string & key, const std::string & value) :
		KeyOnlyTagFilter(key), m_Value(value), m_ValueId(0) {}

	bool StringTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
		if (m_PBI == pbi)
			return !m_PBI || (m_KeyId && m_ValueId);

		m_PBI = pbi;
		return findKeyId() && findValueId();
	}

	void StringTagFilter::setValue (const std::string & value) {
		m_Value = value;
		findValueId();
	}

	bool StringTagFilter::findValueId() {
		m_ValueId = 0;

		if (!m_PBI)
			return true;

		uint32_t stringTableSize = m_PBI->stringTableSize();

		for (m_ValueId = 1; m_ValueId < stringTableSize; ++m_ValueId) {
			if (m_Value == m_PBI->queryStringTable(m_ValueId))
				break;
		}

		if (m_ValueId >= stringTableSize)
			m_ValueId = 0;

		return m_ValueId;
	}

	bool StringTagFilter::p_matches(const IPrimitive & primitive) const {
		if (m_Key.empty())
			return false;

		if (m_PBI)
			return hasTag<IPrimitive>(primitive, m_KeyId, m_ValueId);

		for (int i = 0; i < primitive.tagsSize(); ++i) {
			if ((primitive.key(i) == m_Key) && primitive.value(i) == m_Value)
				return true;
		}

		return false;
	}

	// BoolTagFilter

	BoolTagFilter::BoolTagFilter(const std::string & key, bool value) :
		KeyOnlyTagFilter(key), m_Value(value) {}

	bool BoolTagFilter::p_matches (const IPrimitive & primitive) const {
		if (m_Key.empty())
			return false;

		if (m_Value) {
			for (int i = 0; i < primitive.tagsSize(); ++i) {
				if (
					(m_PBI ? (primitive.keyId(i) == m_KeyId) : (primitive.key(i) == m_Key)) &&
					((primitive.value(i) == "yes") || (primitive.value(i) == "true") || (primitive.value(i) == "1"))
				)
					return true;
			}
		}
		else {
			for (int i = 0; i < primitive.tagsSize(); ++i) {
				if (
					(m_PBI ? (primitive.keyId(i) == m_KeyId) : (primitive.key(i) == m_Key)) &&
					((primitive.value(i) == "no") || (primitive.value(i) == "false") || (primitive.value(i) == "0"))
				)
					return true;
			}
		}

		return false;
	}

	// IntTagFilter

	IntTagFilter::IntTagFilter(const std::string & key, int value) :
		KeyOnlyTagFilter(key), m_Value(value) {}

	bool IntTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
		if (m_PBI == pbi)
			return !m_PBI || (m_KeyId && m_ValueId);

		m_PBI = pbi;
		return findKeyId() && findValueId();
	}

	bool IntTagFilter::findValueId() {
		m_ValueId = 0;

		if (!m_PBI)
			return true;

		uint32_t stringTableSize = m_PBI->stringTableSize();

		for (m_ValueId = 1; m_ValueId < stringTableSize; ++m_ValueId) {
			const std::string & tagValue = m_PBI->queryStringTable(m_ValueId);
			char * endptr;
			int intTagValue = strtol(tagValue.c_str(), &endptr, 10);

			if ((*endptr == '\0') && (intTagValue == m_Value))
				break;
		}

		if (m_ValueId >= stringTableSize)
			m_ValueId = 0;

		return m_ValueId;
	}

	bool IntTagFilter::p_matches(const IPrimitive & primitive) const {
		if (m_Key.empty())
			return false;

		if (m_PBI)
			return hasTag<IPrimitive>(primitive, m_KeyId, m_ValueId);

		for (int i = 0; i < primitive.tagsSize(); ++i) {
			if (primitive.key(i) == m_Key) {
				const std::string & tagValue = m_PBI->queryStringTable(m_ValueId);
				char * endptr;
				int intTagValue = strtol(tagValue.c_str(), &endptr, 10);

				if ((*endptr == '\0') && (intTagValue == m_Value))
					return true;
			}
		}

		return false;
	}

}
