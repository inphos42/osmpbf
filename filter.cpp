#include "filter.h"
#include "iway.h"
#include "inode.h"
#include "irelation.h"

namespace osmpbf {

	// AbstractMultiTagFilter

	AbstractMultiTagFilter::~AbstractMultiTagFilter() {
		for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it)
			delete *it;
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
		AbstractTagFilter(), m_Key(key), m_Value(value) {}

	template <class OSMInputPrimitive>
	bool StringTagFilter::t_matches(const OSMInputPrimitive & primitive) const {
		for (int i = 0; i < primitive.tagsSize(); ++i) {
			if ((primitive.key(i) == m_Key) && ((m_Value.empty()) || (primitive.value(i) == m_Value)))
				return true;
		}

		return false;
	}

}
