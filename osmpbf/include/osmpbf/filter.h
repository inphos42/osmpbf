/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2014 Oliver Groß.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, see
    <http://www.gnu.org/licenses/>.
 */

#ifndef OSMPBF_FILTER_H
#define OSMPBF_FILTER_H

#include <osmpbf/common_input.h>
#include "primitiveblockinputadaptor.h"

#include <generics/macros.h>
#include <generics/refcountobject.h>

#include <forward_list>
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <regex>

/**
  * Filters do what their name suggests.
  * They take as input a primitive and return if the primitive matches the filter specification
  * Essentially a filter is a DAG out of other filters.
  * Inner nodes are boolean operations on sub-nodes (and, or, not).
  * Leaf nodes can be anything from a simple ConstantReturnFilter to RegexKeyTagFilter.
  * See below for a list of filters.
  *
  * Filters are NOT thread-safe!
  * 
  * It is possible to speed up filtering by assigning a PrimitiveBlockInputAdaptor (PBI) to a filter.
  * Note that the assigned PBI needs to be valid during usage of the filter.
  * It is possible to rebuild filter caches after changing the content of a PBI.
  * The return value of the rebuilFilter() function indicates if there may be elements in the block matching the filter
  * You can use the CopyFilterPtr class to handle filter dags with copy-semantic (useful when dealing with multi-threading)
  * Use the RCFilterPtr if reference counting semantics is good enough
  * 
  * 
  */

namespace osmpbf
{

class AndTagFilter;
class OrTagFilter;
class InversionFilter;
class ConstantReturnFilter;
class PrimitiveTypeFilter;
class BoolTagFilter;
class IntTagFilter;
class KeyOnlyTagFilter;
class KeyValueTagFilter;
class KeyMultiValueTagFilter;
class MultiKeyMultiValueTagFilter;
class RegexKeyTagFilter;

template<class OSMInputPrimitive>
int findTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId);

template<class OSMInputPrimitive>
int findKey(const OSMInputPrimitive & primitive, uint32_t keyId);

template<class OSMInputPrimitive>
bool hasTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId);

template<class OSMInputPrimitive>
bool hasKey(const OSMInputPrimitive & primitive, uint32_t keyId);

///This is the base class for all filters
class AbstractTagFilter : public generics::RefCountObject
{
public:
	AbstractTagFilter();
	virtual ~AbstractTagFilter();
	///Create a deep copy of the filter dag. This does not copy assigned PrimitiveBlockInputAdaptors.
	AbstractTagFilter * copy() const;
public:
	///assign an input adaptor to allow for faster filter due to caches
	virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi);
	///manually initiate a cache rebuild.
	///Returns true if there may exist a matching primitive in the currently assigned input adaptor 
	virtual bool rebuildCache();
public:
	///return true if the primitive matches the filter
	bool matches(const IPrimitive & primitive);
protected:
	typedef std::unordered_map<const AbstractTagFilter*, AbstractTagFilter*> CopyMap;
	///sub classes need to implement the private matches function
	virtual bool p_matches(const IPrimitive & primitive) = 0;
protected:
	///create a copy of this instance.
	///@param copies contains copied instances. This has to contain this instance after copying.
	///Usually this can be achieved using copies[this] = MyFilter();
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const = 0;
	AbstractTagFilter * copy(AbstractTagFilter * other, AbstractTagFilter::CopyMap & copies) const;
};

class AbstractMultiTagFilter : public AbstractTagFilter
{
public:
	AbstractMultiTagFilter() : AbstractTagFilter() {}
	virtual ~AbstractMultiTagFilter();
public:
	virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) override;
public:
	AbstractTagFilter * addChild(AbstractTagFilter * child);
	template<typename T_ABSTRACT_TAG_FILTER_ITERATOR>
	void addChildren(T_ABSTRACT_TAG_FILTER_ITERATOR begin, const T_ABSTRACT_TAG_FILTER_ITERATOR & end);
protected:
	typedef std::forward_list<AbstractTagFilter *> FilterList;
	virtual AbstractTagFilter* copy(CopyMap& copies) const override = 0;
protected:
	FilterList m_Children;
};

///This class handles cache consistency for filters with caches
class AbstractTagFilterWithCache : public AbstractTagFilter
{
public:
	AbstractTagFilterWithCache();
	virtual ~AbstractTagFilterWithCache();
public:
	void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) override;
	bool rebuildCache() override;
protected:
	bool p_matches(const IPrimitive & primitive) override;
protected:
	AbstractTagFilterWithCache(const AbstractTagFilterWithCache & other);
	bool dirty() const;
	///sub classes have to call this function if their cache is in-validated
	void markDirty();
	void markClean();
protected:
	///rebuild cache of sub class
	virtual bool p_rebuildCache() = 0;
	///use cache to match primitive
	virtual bool p_cached_match(const IPrimitive & primitive);
	///don't use cache to match primitive
	virtual bool p_uncached_match(const IPrimitive & primitive) = 0;
protected:
	PrimitiveBlockInputAdaptor::IdType m_pbiId;
	const PrimitiveBlockInputAdaptor * m_PBI;
};

///A simple wrapper for filters providing reference counting semantics
typedef generics::RCPtr<AbstractTagFilter> RCFilterPtr;

///A simple wrapper for filter providing copy semantics
class CopyFilterPtr {
private:
	void safe_bool_func() {}
	typedef void (CopyFilterPtr:: * safe_bool_type) ();
public:
	CopyFilterPtr();
	explicit CopyFilterPtr(const RCFilterPtr & other);
	CopyFilterPtr(const CopyFilterPtr & other);
	CopyFilterPtr(CopyFilterPtr && other);
	virtual ~CopyFilterPtr();
	CopyFilterPtr & operator=(const CopyFilterPtr & other);
	CopyFilterPtr & operator=(CopyFilterPtr && other);
	bool operator==(const CopyFilterPtr & other);
	bool operator!=(const CopyFilterPtr & other);
	AbstractTagFilter & operator*();
	const AbstractTagFilter & operator*() const;
	AbstractTagFilter * operator->();
	const AbstractTagFilter * operator->() const;
	AbstractTagFilter * get();
	const AbstractTagFilter * get() const;
	operator safe_bool_type() const;
	void reset(const RCFilterPtr & filter);
	void reset(RCFilterPtr && filter);
private:
	RCFilterPtr & priv();
	const RCFilterPtr priv() const;
private:
	RCFilterPtr m_Private;
};

///Inverts the result of another filter
class InversionFilter: public AbstractTagFilter {
public:
	InversionFilter();
	InversionFilter(AbstractTagFilter * child);
	InversionFilter(const InversionFilter & other) = delete;
	InversionFilter operator=(const InversionFilter & other) = delete;
	virtual ~InversionFilter();
public:
	virtual void assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi) override;
	virtual bool rebuildCache() override;
public:
	void setChild(AbstractTagFilter * child);
	AbstractTagFilter * child();
	const AbstractTagFilter * child() const;
public:
	///invert a given filter (removes InversionFilter if applicable)
	static void invert(RCFilterPtr & filter);
	static RCFilterPtr invert(AbstractTagFilter * filter);
protected:
	virtual bool p_matches(const IPrimitive & primitive) override;
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
private:
	AbstractTagFilter * m_child;
};

//always returns either true or false
class ConstantReturnFilter: public AbstractTagFilter
{
public:
	ConstantReturnFilter(bool returnValue);
	virtual ~ConstantReturnFilter();
public:
	virtual bool rebuildCache() override;
public:
	void setValue(bool returnValue);
	bool value() const;
protected:
	virtual bool p_matches(const IPrimitive & primitive) override;
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	bool m_returnValue;
};

class PrimitiveTypeFilter: public AbstractTagFilterWithCache
{
public:
	PrimitiveTypeFilter(PrimitiveTypeFlags primitiveTypes);
	virtual ~PrimitiveTypeFilter();
public:
	void setFilteredTypes(PrimitiveTypeFlags primitiveTypes);
	int filteredTypes() const;
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	virtual bool p_rebuildCache() override;
	virtual bool p_uncached_match(const IPrimitive & primitive) override;
private:
	int m_filteredPrimitives;
};

class OrTagFilter : public AbstractMultiTagFilter
{
public:
	OrTagFilter() : AbstractMultiTagFilter() {}
	OrTagFilter(std::initializer_list<AbstractTagFilter*> l);
public:
	virtual bool rebuildCache() override;
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
private:
	virtual bool p_matches(const IPrimitive & primitive) override;
};

class AndTagFilter : public AbstractMultiTagFilter
{
public:
	AndTagFilter() : AbstractMultiTagFilter() {}
	AndTagFilter(std::initializer_list<AbstractTagFilter*> l);
public:
	virtual bool rebuildCache() override;
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
private:
	virtual bool p_matches(const IPrimitive & primitive) override;
};

class KeyOnlyTagFilter : public AbstractTagFilterWithCache
{
public:
	KeyOnlyTagFilter(const std::string & key);
public:
	int matchingTag() const;
	void setKey(const std::string & key);
	const std::string & key() const;
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	virtual bool p_rebuildCache() override;
	virtual bool p_cached_match(const IPrimitive & primitive) override;
	virtual bool p_uncached_match(const IPrimitive & primitive) override;
protected:
	uint32_t findId(const std::string & str);
protected:
	std::string m_Key;
	uint32_t m_KeyId;
	int m_LatestMatch;
};

///A single key, single value tag filter
class KeyValueTagFilter : public KeyOnlyTagFilter
{
public:
	KeyValueTagFilter(const std::string & key, const std::string & value);
public:
	void setValue(const std::string & value);
	const std::string & value() const;
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	virtual bool p_rebuildCache() override;
	virtual bool p_cached_match(const IPrimitive & primitive) override;
	virtual bool p_uncached_match(const IPrimitive & primitive) override;
protected:
	std::string m_Value;
	uint32_t m_ValueId;
};

///A single key, multiple values filter
class KeyMultiValueTagFilter : public KeyOnlyTagFilter
{
public:
	typedef std::unordered_set<uint32_t> IdSet;
	typedef std::unordered_set<std::string> ValueSet;
public:
	KeyMultiValueTagFilter(const std::string & key);
	KeyMultiValueTagFilter(const std::string & key, std::initializer_list<std::string> l);
	template<typename T_STRING_ITERATOR>
	KeyMultiValueTagFilter(const std::string & key, const T_STRING_ITERATOR & begin, const T_STRING_ITERATOR & end);
public:
	template<typename T_STRING_ITERATOR>
	void setValues(const T_STRING_ITERATOR & begin, const T_STRING_ITERATOR & end);
	void setValues(const std::set<std::string> & values);
	void setValues(const std::unordered_set<std::string> & values);
	void setValues(std::initializer_list<std::string> l);
	void addValue(const std::string & value);
	KeyMultiValueTagFilter & operator<<(const std::string & value);
	KeyMultiValueTagFilter & operator<<(const char * value);
	void clearValues();
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	virtual bool p_rebuildCache() override;
	virtual bool p_cached_match(const IPrimitive & primitive) override;
	virtual bool p_uncached_match(const IPrimitive & primitive) override;
protected:
	void updateValueIds();
protected:
	IdSet m_IdSet;
	ValueSet m_ValueSet;
};

///A multi-key, any-value filter
class MultiKeyTagFilter : public AbstractTagFilterWithCache
{
public:
	typedef std::unordered_set<uint32_t> IdSet;
	typedef std::unordered_set<std::string> ValueSet;
public:
	MultiKeyTagFilter(std::initializer_list<std::string> l);
	template<typename T_STRING_ITERATOR>
	MultiKeyTagFilter(const T_STRING_ITERATOR & begin, const T_STRING_ITERATOR & end);
public:
	template<typename T_STRING_ITERATOR>
	void addValues(const T_STRING_ITERATOR & begin, const T_STRING_ITERATOR & end);
	void addValues(std::initializer_list<std::string> l);
	void addValue(const std::string & value);
	MultiKeyTagFilter & operator<<(const std::string & value);
	MultiKeyTagFilter & operator<<(const char * value);
	void clearValues();
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	virtual bool p_rebuildCache() override;
	virtual bool p_cached_match(const IPrimitive & primitive) override;
	virtual bool p_uncached_match(const IPrimitive & primitive) override;
protected:
	IdSet m_IdSet;
	ValueSet m_KeySet;
};

///A multiple keys, multiple-values filter
class MultiKeyMultiValueTagFilter : public AbstractTagFilter
{
public:
	MultiKeyMultiValueTagFilter();
public:
	template<typename T_STRING_ITERATOR>
	void addValues(const std::string & key, const T_STRING_ITERATOR & begin, const T_STRING_ITERATOR & end);
	void clearValues();
protected:
	typedef std::unordered_map<std::string, std::unordered_set<std::string> > ValueMap;
protected:
	virtual bool p_matches(const IPrimitive & primitive) override;
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	ValueMap m_ValueMap;
};

///A regex based key filter.
class RegexKeyTagFilter : public AbstractTagFilterWithCache
{
public:
	RegexKeyTagFilter();
	RegexKeyTagFilter(const std::string & regexString, std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
	RegexKeyTagFilter(const std::regex & regex, std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
	template<typename T_OCTET_ITERATOR>
	RegexKeyTagFilter(const T_OCTET_ITERATOR & begin, const T_OCTET_ITERATOR & end, std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
	virtual ~RegexKeyTagFilter();
public:
	template<typename T_OCTET_ITERATOR>
	void setRegex(const T_OCTET_ITERATOR & begin, const T_OCTET_ITERATOR & end, std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
	void setRegex(const std::regex & regex, std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
	void setRegex(const std::string & regexString, std::regex_constants::match_flag_type flags = std::regex_constants::match_default);
protected:
	virtual bool p_rebuildCache() override;
	virtual bool p_cached_match(const IPrimitive & primitive) override;
	virtual bool p_uncached_match(const IPrimitive & primitive) override;
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	std::regex m_regex;
	std::regex_constants::match_flag_type m_matchFlags;
	std::unordered_set<int> m_IdSet;
private:
	RegexKeyTagFilter(std::regex_constants::match_flag_type flags);
};

/** Check for a @key that matches boolean value @value. Evaluates to false if key is not available */
class BoolTagFilter : public KeyMultiValueTagFilter
{
public:
	BoolTagFilter(const std::string & key, bool value);
public:
	void setValue(bool value);
	bool value() const;
private:
	void setValues(const std::set< std::string > & values) = delete;
	void addValue(const std::string & value);
	void clearValues();
	KeyMultiValueTagFilter & operator<<(const std::string & value);
	KeyMultiValueTagFilter & operator<<(const char * value);
protected:
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
	bool m_Value;
};

class IntTagFilter : public KeyOnlyTagFilter
{
public:
	IntTagFilter(const std::string & key, int value);
public:
	void setValue(int value);
	int value() const;
protected:
	virtual bool p_rebuildCache() override;
	virtual bool p_cached_match(const IPrimitive & primitive) override;
	virtual bool p_uncached_match(const IPrimitive & primitive) override;
	virtual AbstractTagFilter * copy(AbstractTagFilter::CopyMap & copies) const override;
protected:
	bool findValueId();
protected:
	int m_Value;
	uint32_t m_ValueId;
};

AndTagFilter * newAnd(AbstractTagFilter * a, AbstractTagFilter * b);
OrTagFilter * newOr(AbstractTagFilter * a, AbstractTagFilter * b);

//definitions

template<class OSMInputPrimitive>
int findTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId)
{
	if (!keyId || !valueId)
		return -1;

	for (int i = 0; i < primitive.tagsSize(); i++)
		if (primitive.keyId(i) == keyId && primitive.valueId(i) == valueId)
			return i;

	return -1;
}

template<class OSMInputPrimitive>
int findKey(const OSMInputPrimitive & primitive, uint32_t keyId)
{
	if (!keyId)
		return -1;

	for (int i = 0; i < primitive.tagsSize(); ++i)
		if (primitive.keyId(i) == keyId)
			return i;

	return -1;
}

template<class OSMInputPrimitive>
bool hasTag(const OSMInputPrimitive & primitive, uint32_t keyId, uint32_t valueId)
{
	return findTag<OSMInputPrimitive>(primitive, keyId, valueId) > -1;
}

template<class OSMInputPrimitive>
bool hasKey(const OSMInputPrimitive & primitive, uint32_t keyId)
{
	return findKey<OSMInputPrimitive>(primitive, keyId) > -1;
}

template<typename T_ABSTRACT_TAG_FILTER_ITERATOR>
void AbstractMultiTagFilter::addChildren(T_ABSTRACT_TAG_FILTER_ITERATOR begin, const T_ABSTRACT_TAG_FILTER_ITERATOR & end)
{
	for (; begin != end; ++begin)
	{
		addChild(*begin);
	}
}

template<typename T_STRING_ITERATOR>
KeyMultiValueTagFilter::KeyMultiValueTagFilter(const std::string & key, const T_STRING_ITERATOR & begin, const T_STRING_ITERATOR & end) :
KeyOnlyTagFilter(key),
m_ValueSet(begin, end)
{
	updateValueIds();
}


template<typename T_STRING_ITERATOR>
void KeyMultiValueTagFilter::setValues(const T_STRING_ITERATOR & begin, const T_STRING_ITERATOR & end)
{
	m_ValueSet.clear();
	m_ValueSet.insert(begin, end);
	updateValueIds();
}

template<typename T_STRING_ITERATOR>
MultiKeyTagFilter::MultiKeyTagFilter(const T_STRING_ITERATOR& begin, const T_STRING_ITERATOR& end) :
m_KeySet(begin, end)
{}

template<typename T_STRING_ITERATOR>
void MultiKeyTagFilter::addValues(const T_STRING_ITERATOR& begin, const T_STRING_ITERATOR& end)
{
	m_KeySet.insert(begin, end);
	markDirty();
}

template<typename T_STRING_ITERATOR>
void MultiKeyMultiValueTagFilter::addValues(const std::string& key, const T_STRING_ITERATOR& begin, const T_STRING_ITERATOR& end)
{
	m_ValueMap[key].insert(begin, end);
}

template<typename T_OCTET_ITERATOR>
RegexKeyTagFilter::RegexKeyTagFilter(const T_OCTET_ITERATOR & begin, const T_OCTET_ITERATOR & end, std::regex_constants::match_flag_type flags) :
m_regex(begin, end),
m_matchFlags(flags)
{}

template<typename T_OCTET_ITERATOR>
void RegexKeyTagFilter::setRegex(const T_OCTET_ITERATOR & begin, const T_OCTET_ITERATOR & end, std::regex_constants::match_flag_type flags) {
	m_regex.assign(begin, end);
	m_matchFlags = flags;
	markDirty();
}

} // namespace osmpbf

#endif // OSMPBF_FILTER_H
