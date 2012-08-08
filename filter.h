#ifndef OSMPBF_FILTER_H
#define OSMPBF_FILTER_H

#include <forward_list>
#include <string>
#include <set>

#include "refcountobject.h"

namespace osmpbf {

	template<class OSMInputPrimitive>
	int findTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId) {
		if (!keyId || !valueId)
			return -1;

		for (int i = 0; i < primitive.tagsSize(); i++)
			if (primitive.keyId(i) == keyId && primitive.valueId(i) == valueId)
				return i;

		return -1;
	}

	template<class OSMInputPrimitive>
	int findKey(const OSMInputPrimitive & primitive, uint32_t keyId) {
		if (!keyId)
			return -1;

		for (int i = 0; i < primitive.tagsSize(); ++i)
			if (primitive.keyId(i) == keyId)
				return i;

		return -1;
	}

	template<class OSMInputPrimitive>
	inline bool hasTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId) {
		return findTag<OSMInputPrimitive>(primitive, keyId, valueId) > -1;
	}

	template<class OSMInputPrimitive>
	inline bool hasKey(const OSMInputPrimitive & primitive, uint32_t keyId) {
		return findKey<OSMInputPrimitive>(primitive, keyId) > -1;
	}

	class IPrimitive;
	class PrimitiveBlockInputAdaptor;

	class AbstractTagFilter : public RefCountObject {
	public:
		AbstractTagFilter() : RefCountObject(), m_Invert(false) {}
		virtual ~AbstractTagFilter() {}

		inline bool matches(const IPrimitive & primitive) { return m_Invert ? !p_matches(primitive) : p_matches(primitive); }

		virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) = 0;
		virtual bool buildIdCache() = 0;

		inline bool invert() { m_Invert = !m_Invert; return m_Invert; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive) = 0;

		bool m_Invert;
	};

	class AbstractMultiTagFilter : public AbstractTagFilter {
	public:
		AbstractMultiTagFilter() : AbstractTagFilter() {}
		virtual ~AbstractMultiTagFilter();

		virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi);
		virtual bool buildIdCache();

		inline AbstractTagFilter * addChild(AbstractTagFilter * child) {
			m_Children.push_front(child);
			child->rcInc();
			return child;
		}

	protected:
		typedef std::forward_list<AbstractTagFilter *> FilterList;

		FilterList m_Children;
	};

	class OrTagFilter : public AbstractMultiTagFilter {
	public:
		OrTagFilter() : AbstractMultiTagFilter() {}

	private:
		virtual bool p_matches(const IPrimitive & primitive);
	};

	class AndTagFilter : public AbstractMultiTagFilter {
	public:
		AndTagFilter() : AbstractMultiTagFilter() {}

	private:
		virtual bool p_matches(const IPrimitive & primitive);
	};

	class KeyOnlyTagFilter : public AbstractTagFilter {
	public:
		KeyOnlyTagFilter(const std::string & key);

		virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
			if (m_PBI != pbi) m_KeyIdIsDirty = true;
			m_PBI = pbi;
		}
		virtual bool buildIdCache();

		int matchingTag() const { return m_LatestMatch; }

		void setKey(const std::string & key);

		inline const std::string & key() const { return m_Key; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive);

		uint32_t findId(const std::string & str);

		std::string m_Key;

		uint32_t m_KeyId;
		bool m_KeyIdIsDirty;

		inline void checkKeyIdCache() {
			if (m_KeyIdIsDirty) {
				m_KeyId = findId(m_Key);
				m_KeyIdIsDirty = false;
			}
		}

		int m_LatestMatch;

		const osmpbf::PrimitiveBlockInputAdaptor * m_PBI;
	};

	class StringTagFilter : public KeyOnlyTagFilter {
	public:
		StringTagFilter(const std::string & key, const std::string & value);

		virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
			if (m_PBI != pbi) { m_KeyIdIsDirty = true; m_ValueIdIsDirty = true; }
			m_PBI = pbi;
		}
		virtual bool buildIdCache();

		void setValue(const std::string & value);

		inline const std::string & Value() const { return m_Value; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive);

		std::string m_Value;

		uint32_t m_ValueId;
		bool m_ValueIdIsDirty;

		inline void checkValueIdCache() {
			if (m_ValueIdIsDirty) {
				m_ValueId = findId(m_Value);
				m_ValueIdIsDirty = false;
			}
		}
	};

	class MultiStringTagFilter : public KeyOnlyTagFilter {
	public:
		MultiStringTagFilter (const std::string & key);

		virtual bool buildIdCache();

		void setValues(const std::set< std::string > & values);
		void addValue(const std::string & value);
		void clearValues() { m_IdSet.clear(); m_ValueSet.clear(); }

		inline MultiStringTagFilter & operator<<(const std::string & value) { addValue(value); return *this; }
		inline MultiStringTagFilter & operator<<(const char * value) { addValue(value); return *this; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive);

		void updateValueIds();

		std::set< uint32_t > m_IdSet;
		std::set< std::string > m_ValueSet;
	};

	class BoolTagFilter : protected MultiStringTagFilter {
	public:
		BoolTagFilter(const std::string & key, bool value);

		virtual bool buildIdCache() { return MultiStringTagFilter::buildIdCache(); }

		void setValue(bool value);
		inline bool value() const { return m_Value; }

	protected:
		bool m_Value;
	};

	class IntTagFilter : public KeyOnlyTagFilter {
	public:
		IntTagFilter(const std::string & key, int value);

		virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) {
			if (m_PBI != pbi) { m_KeyIdIsDirty = true; m_ValueIdIsDirty = true; }
			m_PBI = pbi;
		}
		virtual bool buildIdCache();

		void setValue(int value);
		inline int value() const { return m_Value; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive);

		bool findValueId();

		int m_Value;
		uint32_t m_ValueId;
		bool m_ValueIdIsDirty;

		inline void checkValueIdCache() {
			if (m_ValueIdIsDirty)
				findValueId();
		}
	};

}

#endif
