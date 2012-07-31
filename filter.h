#ifndef OSMPBF_FILTER_H
#define OSMPBF_FILTER_H

#include <forward_list>
#include <string>

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

	class AbstractTagFilter {
	public:
		AbstractTagFilter() : m_Invert(false) {}
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

		inline void addChild(AbstractTagFilter * child) { m_Children.push_front(child); }

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

	class StringTagFilter : public AbstractTagFilter {
	public:
		StringTagFilter(const std::string & key, const std::string & value);

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL);

		void setKey(const std::string & key);
		void setValue(const std::string & value);

		inline const std::string & key() const { return m_Key; }
		inline const std::string & Value() const { return m_Value; }

	private:
		StringTagFilter();

		virtual bool p_matches(const IPrimitive & primitive) const;

		std::string m_Key;
		std::string m_Value;

		uint32_t m_KeyId;
		uint32_t m_ValueId;

		bool m_IdsOnly;
	};

}

#endif
