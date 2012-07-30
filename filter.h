#ifndef OSMPBF_FILTER_H
#define OSMPBF_FILTER_H

#include <forward_list>
#include <string>

namespace osmpbf {

	template<class OSMInputPrimitive>
	bool hasTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId) {
		for (int i = 0; i < primitive.tagsSize(); i++)
			if (primitive.keyId(i) == keyId && (valueId == 0 || primitive.valueId(i) == valueId))
				return true;

		return false;
	}

	class IWay;
	class INode;
	class IRelation;
	class PrimitiveBlockInputAdaptor;

	class AbstractTagFilter {
	public:
		AbstractTagFilter() {}
		virtual ~AbstractTagFilter() {}

		virtual bool matches(const IWay & way) const = 0;
		virtual bool matches(const INode & node) const = 0;
		virtual bool matches(const IRelation & relation) const = 0;

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL) = 0;
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

		virtual bool matches(const IWay & way) const { return t_matches<IWay>(way); }
		virtual bool matches(const INode & node) const { return t_matches<INode>(node); }
		virtual bool matches(const IRelation & relation) const { return t_matches<IRelation>(relation); }

	private:
		template <class OSMInputPrimitive>
		bool t_matches(const OSMInputPrimitive & primitive) const;
	};

	class AndTagFilter : public AbstractMultiTagFilter {
	public:
		AndTagFilter() : AbstractMultiTagFilter() {}

		virtual bool matches(const IWay & way) const { return t_matches<IWay>(way); }
		virtual bool matches(const INode & node) const { return t_matches<INode>(node); }
		virtual bool matches(const IRelation & relation) const { return t_matches<IRelation>(relation); }

	private:
		template <class OSMInputPrimitive>
		bool t_matches(const OSMInputPrimitive & primitive) const;
	};

	class StringTagFilter : public AbstractTagFilter {
	public:
		StringTagFilter(const std::string & key, const std::string & value);

		virtual bool matches(const IWay & way) const { return t_matches<IWay>(way); }
		virtual bool matches(const INode & node) const { return t_matches<INode>(node); }
		virtual bool matches(const IRelation & relation) const { return t_matches<IRelation>(relation); }

		virtual bool assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi = NULL);

		void setKey(const std::string & key);
		void setValue(const std::string & value);

	private:
		StringTagFilter();

		template <class OSMInputPrimitive>
		bool t_matches(const OSMInputPrimitive & primitive) const;

		std::string m_Key;
		std::string m_Value;

		int m_KeyId;
		int m_ValueId;

		bool m_IdsOnly;
	};

}

#endif
