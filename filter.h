#ifndef OSMPBF_FILTER_H
#define OSMPBF_FILTER_H

#include <forward_list>
#include <string>
#include <set>

#include "refcountobject.h"

namespace osmpbf {

	template<class OSMInputPrimitive>
	bool hasTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId) {
		if (!keyId || !valueId)
			return false;

		for (int i = 0; i < primitive.tagsSize(); i++)
			if (primitive.keyId(i) == keyId && primitive.valueId(i) == valueId)
				return true;

		return false;
	}

	template<class OSMInputPrimitive>
	bool hasKey(const OSMInputPrimitive & primitive, uint32_t keyId) {
		if (!keyId)
			return false;

		for (int i = 0; i < primitive.tagsSize(); i++)
			if (primitive.keyId(i) == keyId)
				return true;

		return false;
	}

	class IPrimitive;
	class PrimitiveBlockInputAdaptor;

	class AbstractTagFilter : public RefCountObject {
	public:
		AbstractTagFilter() : RefCountObject(), m_Invert(false) {}
		virtual ~AbstractTagFilter() {}

		inline bool matches(const IPrimitive & primitive) const { return m_Invert ? !p_matches(primitive) : p_matches(primitive); }

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL) = 0;

		inline bool invert() { m_Invert = !m_Invert; return m_Invert; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive) const = 0;

		bool m_Invert;
	};

	class AbstractMultiTagFilter : public AbstractTagFilter {
	public:
		AbstractMultiTagFilter() : AbstractTagFilter() {}
		virtual ~AbstractMultiTagFilter();

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL);

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
		virtual bool p_matches(const IPrimitive & primitive) const;
	};

	class AndTagFilter : public AbstractMultiTagFilter {
	public:
		AndTagFilter() : AbstractMultiTagFilter() {}

	private:
		virtual bool p_matches(const IPrimitive & primitive) const;
	};

	class KeyOnlyTagFilter : public AbstractTagFilter {
	public:
		KeyOnlyTagFilter(const std::string & key);

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL);

		void setKey(const std::string & key);

		inline const std::string & key() const { return m_Key; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive) const;

		uint32_t findId(const std::string & str);

		std::string m_Key;

		uint32_t m_KeyId;

		const osmpbf::PrimitiveBlockInputAdaptor * m_PBI;
	};

	class StringTagFilter : public KeyOnlyTagFilter {
	public:
		StringTagFilter(const std::string & key, const std::string & value);

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL);

		void setValue(const std::string & value);

		inline const std::string & Value() const { return m_Value; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive) const;

		std::string m_Value;

		uint32_t m_ValueId;
	};

	class MultiStringTagFilter : public KeyOnlyTagFilter {
	public:
		MultiStringTagFilter (const std::string & key);

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL);

		void setValues(const std::set< std::string > & values);
		void addValue(const std::string & value);
		void clearValues() { m_IdSet.clear(); m_ValueSet.clear(); }

	protected:
		virtual bool p_matches(const IPrimitive & primitive) const;

		void updateValueIds();

		std::set< uint32_t > m_IdSet;
		std::set< std::string > m_ValueSet;
	};

	class BoolTagFilter : public KeyOnlyTagFilter {
	public:
		BoolTagFilter(const std::string & key, bool value);

		void setValue(bool value) { m_Value = value; }
		inline bool value() const { return m_Value; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive) const;

		bool m_Value;
	};

	class IntTagFilter : public KeyOnlyTagFilter {
	public:
		IntTagFilter(const std::string & key, int value);

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL);

		void setValue(int value);
		inline int value() const { return m_Value; }

	protected:
		virtual bool p_matches(const IPrimitive & primitive) const;
		bool findValueId();

		int m_Value;
		uint32_t m_ValueId;
	};

}

#endif
