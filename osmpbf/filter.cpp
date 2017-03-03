/*
    This file is part of the osmpbf library.

    Copyright(c) 2012-2013 Oliver Groß.

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

#include <osmpbf/filter.h>

#include <osmpbf/primitiveblockinputadaptor.h>
#include <osmpbf/iprimitive.h>
#include <osmpbf/irelation.h>

#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <array>

namespace osmpbf
{

AbstractTagFilter::AbstractTagFilter() :
generics::RefCountObject()
{}

AbstractTagFilter::~AbstractTagFilter()
{}

bool AbstractTagFilter::matches(const IPrimitive & primitive)
{
	return p_matches(primitive);
}

AbstractTagFilter* AbstractTagFilter::copy() const
{
	AbstractTagFilter::CopyMap cm;
	return this->copy(cm);
}

AbstractTagFilter * AbstractTagFilter::copy(AbstractTagFilter * other, AbstractTagFilter::CopyMap & copies) const
{
	return other->copy(copies);
}

CopyFilterPtr::CopyFilterPtr()
{}

CopyFilterPtr::CopyFilterPtr(const RCFilterPtr & other) :
m_Private(other->copy())
{}

CopyFilterPtr::CopyFilterPtr(const CopyFilterPtr & other) :
m_Private(other->copy())
{}

CopyFilterPtr::CopyFilterPtr(CopyFilterPtr && other) :
m_Private(std::move(other.m_Private))
{}

CopyFilterPtr::~CopyFilterPtr()
{}

CopyFilterPtr & CopyFilterPtr::operator=(const CopyFilterPtr & other)
{
	m_Private.reset( other->copy() );
	return *this;
}

CopyFilterPtr & CopyFilterPtr::operator=(CopyFilterPtr && other)
{
	m_Private = std::move(other.m_Private);
	return *this;
}

bool CopyFilterPtr::operator==(const CopyFilterPtr & other)
{
	return m_Private == other.m_Private;
}

bool CopyFilterPtr::operator!=(const CopyFilterPtr & other)
{
	return m_Private != other.m_Private;
}

AbstractTagFilter & CopyFilterPtr::operator*()
{
	return *priv();
}

const AbstractTagFilter & CopyFilterPtr::operator*() const
{
	return *priv();
}

AbstractTagFilter * CopyFilterPtr::operator->()
{
	return priv().operator->();
}

const AbstractTagFilter * CopyFilterPtr::operator->() const
{
	return priv().operator->();
}

AbstractTagFilter * CopyFilterPtr::get()
{
	return priv().get();
}

const AbstractTagFilter * CopyFilterPtr::get() const
{
	return priv().get();
}

CopyFilterPtr::operator safe_bool_type() const
{
	return get() ? &CopyFilterPtr::safe_bool_func : 0;
}

void CopyFilterPtr::reset(const RCFilterPtr & filter)
{
	m_Private.reset( filter->copy() );
}

void CopyFilterPtr::reset(RCFilterPtr && filter)
{
	m_Private = std::move(filter);
}

RCFilterPtr & CopyFilterPtr::priv()
{
	return m_Private;
}

const RCFilterPtr CopyFilterPtr::priv() const
{
	return m_Private;
}


//ConstantReturnFilter
ConstantReturnFilter::ConstantReturnFilter(bool returnValue) :
m_returnValue(returnValue)
{}

ConstantReturnFilter::~ConstantReturnFilter() {}

void ConstantReturnFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor *) {}

bool ConstantReturnFilter::rebuildCache() { return m_returnValue;}

bool ConstantReturnFilter::p_matches(const IPrimitive & /*primitive*/)
{
	return m_returnValue;
}

AbstractTagFilter * ConstantReturnFilter::copy(AbstractTagFilter::CopyMap & copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}

	ConstantReturnFilter * myCopy = new ConstantReturnFilter(m_returnValue);
	copies[this] = myCopy;
	return myCopy;
}

//PrimitiveTypeFilter
PrimitiveTypeFilter::PrimitiveTypeFilter(PrimitiveTypeFlags primitiveTypes) : m_filteredPrimitives(primitiveTypes) {}

PrimitiveTypeFilter::~PrimitiveTypeFilter() {}

void PrimitiveTypeFilter::setFilteredTypes(PrimitiveTypeFlags primitiveTypes)
{
	m_filteredPrimitives = primitiveTypes;
}

int PrimitiveTypeFilter::filteredTypes() const
{
	return m_filteredPrimitives;
}

void PrimitiveTypeFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi)
{
	m_PBI = pbi;
}

AbstractTagFilter* PrimitiveTypeFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}

	PrimitiveTypeFilter * myCopy = new PrimitiveTypeFilter(m_filteredPrimitives);
	copies[this] = myCopy;
	return myCopy;
}

bool PrimitiveTypeFilter::rebuildCache()
{
	if (m_PBI)
	{
		int availableTypes = NoPrimitive;
		if (m_PBI->nodesSize())
			availableTypes |= NodePrimitive;
		if (m_PBI->waysSize())
			availableTypes |= WayPrimitive;
		if (m_PBI->relationsSize())
			availableTypes |= RelationPrimitive;
		return availableTypes & m_filteredPrimitives;
	}
	return true;
}

bool PrimitiveTypeFilter::p_matches(const IPrimitive & primitive)
{
	return primitive.type() & m_filteredPrimitives;
}

// AbstractMultiTagFilter

AbstractMultiTagFilter::~AbstractMultiTagFilter()
{
	for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it)
	{
		(*it)->rcDec();
	}
}

void AbstractMultiTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi)
{
	for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it)
	{
		(*it)->assignInputAdaptor(pbi);
	}
}

AbstractTagFilter * AbstractMultiTagFilter::addChild(AbstractTagFilter * child)
{
	if (child)
	{
		m_Children.push_front(child);
		child->rcInc();
	}
	return child;
}


// OrTagFilter

OrTagFilter::OrTagFilter(std::initializer_list<AbstractTagFilter*> l)
{
	for(auto x : l) {
		addChild(x);
	}
}

bool OrTagFilter::rebuildCache()
{
	bool result = false;
	for (FilterList::const_iterator it(m_Children.cbegin()), end(m_Children.cend()); it != end; ++it)
	{
		result = (*it)->rebuildCache() || result;
	}

	return result;
}

bool OrTagFilter::p_matches(const IPrimitive & primitive)
{
	for (FilterList::const_iterator it(m_Children.cbegin()), end(m_Children.cend()); it != end; ++it)
	{
		if ((*it)->matches(primitive))
		{
			return true;
		}
	}

	return false;
}

AbstractTagFilter* OrTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	OrTagFilter * myCopy = new OrTagFilter();
	for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it)
	{
		if (copies.count(*it))
		{
			myCopy->m_Children.push_front(copies.at(*it));
		}
		else
		{
			myCopy->m_Children.push_front( AbstractTagFilter::copy(*it, copies));
		}
		myCopy->m_Children.front()->rcInc();
	}
	myCopy->m_Children.reverse();
	copies[this] = myCopy;
	return myCopy;
}


// AndTagFilter

AndTagFilter::AndTagFilter(std::initializer_list<AbstractTagFilter*> l)
{
	for(auto x : l) {
		addChild(x);
	}
}

bool AndTagFilter::rebuildCache()
{
	bool result = true;
	for (FilterList::const_iterator it(m_Children.cbegin()), end(m_Children.cend()); it != end; ++it)
	{
		result = (*it)->rebuildCache() && result;
	}
	return result;
}

bool AndTagFilter::p_matches(const IPrimitive & primitive)
{
	for (FilterList::const_iterator it(m_Children.cbegin()), end(m_Children.cend()); it != end; ++it)
	{
		if (!(*it)->matches(primitive))
		{
			return false;
		}
	}

	return true;
}

AbstractTagFilter* AndTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	AndTagFilter * myCopy = new AndTagFilter();
	for (FilterList::const_iterator it = m_Children.cbegin(); it != m_Children.cend(); ++it)
	{
		if (copies.count(*it))
		{
			myCopy->m_Children.push_front(copies.at(*it));
		}
		else
		{
			myCopy->m_Children.push_front( AbstractTagFilter::copy(*it, copies));
		}
		myCopy->m_Children.front()->rcInc();
	}
	myCopy->m_Children.reverse();
	copies[this] = myCopy;
	return myCopy;
}


// KeyOnlyTagFilter

KeyOnlyTagFilter::KeyOnlyTagFilter(const std::string & key) :
	AbstractTagFilter(), m_Key(key), m_KeyId(0), m_KeyIdIsDirty(false), m_PBI(NULL) {}

void KeyOnlyTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi)
{
	if (m_PBI != pbi) m_KeyIdIsDirty = true;
	m_PBI = pbi;
}

bool KeyOnlyTagFilter::rebuildCache()
{
	m_KeyId = findId(m_Key);
	m_KeyIdIsDirty = false;

	if (!m_PBI) return true;
	if (m_PBI->isNull()) return false;

	return  m_KeyId;
}

int KeyOnlyTagFilter::matchingTag() const
{
	return m_LatestMatch;
}

void KeyOnlyTagFilter::setKey(const std::string & key)
{
	m_Key = key;
	m_KeyIdIsDirty = true;
}

const std::string & KeyOnlyTagFilter::key() const
{
	return m_Key;
}

bool KeyOnlyTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;
		
	if (m_PBI == primitive.controller())
	{
		if (m_PBI->isNull())
			return false;

		checkKeyIdCache();

		m_LatestMatch = findKey< IPrimitive >(primitive, m_KeyId);
		return m_LatestMatch > -1;
	}
	else
	{
		m_LatestMatch = -1;

		for (int i = 0; i < primitive.tagsSize(); ++i)
		{
			if ((primitive.key(i) == m_Key))
			{
				m_LatestMatch = i;
				return true;
			}
		}

		return false;
	}
}

uint32_t KeyOnlyTagFilter::findId(const std::string & str)
{
	if (!m_PBI || m_PBI->isNull())
		return 0;

	uint32_t id = 0;

	uint32_t stringTableSize = m_PBI->stringTableSize();

	for (id = 1; id < stringTableSize; ++id)
	{
		if (str == m_PBI->queryStringTable(id))
			break;
	}

	if (id >= stringTableSize)
		id = 0;

	return id;
}

void KeyOnlyTagFilter::checkKeyIdCache()
{
	if (m_KeyIdIsDirty)
	{
		m_KeyId = findId(m_Key);
		m_KeyIdIsDirty = false;
	}
}

AbstractTagFilter* KeyOnlyTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	KeyOnlyTagFilter * myCopy = new KeyOnlyTagFilter(key());
	copies[this] = myCopy;
	return myCopy;
}


// StringTagFilter

KeyValueTagFilter::KeyValueTagFilter(const std::string & key, const std::string & value) :
KeyOnlyTagFilter(key),
m_Value(value),
m_ValueId(0),
m_ValueIdIsDirty(false)
{}

void KeyValueTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi)
{
	if (m_PBI != pbi)
	{
		m_KeyIdIsDirty = true;
		m_ValueIdIsDirty = true;
	}
	m_PBI = pbi;
}

bool KeyValueTagFilter::rebuildCache()
{
	m_KeyId = findId(m_Key);
	m_KeyIdIsDirty = false;

	m_ValueId = findId(m_Value);
	m_ValueIdIsDirty = false;

	if (!m_PBI) return true;
	if (m_PBI->isNull()) return false;

	return m_KeyId && m_ValueId;
}

void KeyValueTagFilter::setValue(const std::string & value)
{
	m_Value = value;
	m_ValueIdIsDirty = true;
}

const std::string & KeyValueTagFilter::value() const
{
	return m_Value;
}

bool KeyValueTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;

	if (m_PBI == primitive.controller())
	{
		if (m_PBI->isNull())
			return false;

		checkKeyIdCache();
		checkValueIdCache();

		m_LatestMatch = findTag<IPrimitive>(primitive, m_KeyId, m_ValueId);
		return m_LatestMatch > -1;
	}

	m_LatestMatch = -1;

	for (int i = 0; i < primitive.tagsSize(); ++i)
	{
		if ((primitive.key(i) == m_Key) && primitive.value(i) == m_Value)
		{
			m_LatestMatch = i;
			return true;
		}
	}

	return false;
}

AbstractTagFilter* KeyValueTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	KeyValueTagFilter * myCopy = new KeyValueTagFilter(key(), value());
	copies[this] = myCopy;
	return myCopy;
}

void KeyValueTagFilter::checkValueIdCache()
{
	if (m_ValueIdIsDirty)
	{
		m_ValueId = findId(m_Value);
		m_ValueIdIsDirty = false;
	}
}

// MultiStringTagFilter

KeyMultiValueTagFilter::KeyMultiValueTagFilter(const std::string & key) : KeyOnlyTagFilter(key) {}

KeyMultiValueTagFilter::KeyMultiValueTagFilter(const std::string & key, std::initializer_list<std::string> l) :
KeyOnlyTagFilter(key),
m_ValueSet(l)
{
	updateValueIds();
}

bool KeyMultiValueTagFilter::rebuildCache()
{
	m_KeyId = findId(m_Key);
	m_KeyIdIsDirty = false;

	updateValueIds();

	if (!m_PBI) return true;
	if (m_PBI->isNull()) return false;

	return m_KeyId && m_IdSet.size();
}


void KeyMultiValueTagFilter::setValues(const std::set<std::string> & values)
{
	setValues(values.cbegin(), values.cend());
}

void KeyMultiValueTagFilter::setValues(const std::unordered_set<std::string> & values)
{
	setValues(values.cbegin(), values.cend());
}

void KeyMultiValueTagFilter::setValues(std::initializer_list<std::string> l)
{
	setValues(l.begin(), l.end());
}

void KeyMultiValueTagFilter::addValue(const std::string & value)
{
	m_ValueSet.insert(value);

	uint32_t valueId = findId(value);

	if (valueId)
		m_IdSet.insert(valueId);
}

void KeyMultiValueTagFilter::clearValues()
{
	m_IdSet.clear();
	m_ValueSet.clear();
}

KeyMultiValueTagFilter & KeyMultiValueTagFilter::operator<<(const std::string & value)
{
	addValue(value);
	return *this;
}

KeyMultiValueTagFilter & KeyMultiValueTagFilter::operator<<(const char * value)
{
	addValue(value);
	return *this;
}

bool KeyMultiValueTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;

	m_LatestMatch = -1;

	if (m_PBI == primitive.controller())
	{
		if (m_PBI->isNull())
			return false;

		checkKeyIdCache();

		if (!m_KeyId || m_IdSet.empty())
			return false;

		for (int i = 0; i < primitive.tagsSize(); i++)
		{
			if (primitive.keyId(i) == m_KeyId && m_IdSet.count(primitive.valueId(i)))
			{
				m_LatestMatch = -1;
				return true;
			}
		}

		return false;
	}
	else
	{
		for (int i = 0; i < primitive.tagsSize(); i++)
		{
			if (primitive.key(i) == m_Key && m_ValueSet.count(primitive.value(i)))
			{
				m_LatestMatch = -1;
				return true;
			}
		}

		return false;
	}
}

void KeyMultiValueTagFilter::updateValueIds()
{
	m_IdSet.clear();

	uint32_t valueId = 0;
	for (ValueSet::const_iterator it = m_ValueSet.cbegin(); it != m_ValueSet.cend(); ++it)
	{
		valueId = findId(*it);

		if (valueId)
			m_IdSet.insert(valueId);
	}
}

AbstractTagFilter* KeyMultiValueTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	KeyMultiValueTagFilter * myCopy = new KeyMultiValueTagFilter(key(), m_ValueSet.begin(), m_ValueSet.end());
	copies[this] = myCopy;
	return myCopy;
}

//MultiKeyTagFilter
MultiKeyTagFilter::MultiKeyTagFilter(std::initializer_list< std::string > l) :
m_PBI(0),
m_KeyIdIsDirty(true),
m_KeySet(l.begin(), l.end())
{}

bool MultiKeyTagFilter::rebuildCache()
{
	m_IdSet.clear();
	m_KeyIdIsDirty = false;
	
	if(!m_PBI)
	{
		return true;
	}
	
	if (m_PBI->isNull())
	{
		return false;
	}
		
	for(int i(0), s(m_PBI->stringTableSize()); i < s; ++i)
	{
		if (m_KeySet.count(m_PBI->queryStringTable(i)))
		{
			m_IdSet.insert(i);
		}
	}
	return m_IdSet.size();
}

void MultiKeyTagFilter::addValues(std::initializer_list<std::string> l)
{
	addValues(l.begin(), l.end());
}

void MultiKeyTagFilter::addValue(const std::string& value)
{
	m_KeySet.insert(value);
	m_KeyIdIsDirty = true;
}

MultiKeyTagFilter & MultiKeyTagFilter::operator<<(const std::string & value)
{
	addValue(value);
	return *this;
}

MultiKeyTagFilter & MultiKeyTagFilter::operator<<(const char * value)
{
	addValue(value);
	return *this;
}

void MultiKeyTagFilter::clearValues()
{
	m_IdSet.clear();
	m_KeySet.clear();
	m_KeyIdIsDirty = false;
}

void MultiKeyTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor* pbi)
{
	if (m_PBI != pbi)
	{
		m_KeyIdIsDirty = true;
		m_PBI = pbi;
	}
}

bool MultiKeyTagFilter::p_matches(const IPrimitive& primitive)
{
	if (m_KeySet.empty())
	{
		return false;
	}
	
	if (m_PBI == primitive.controller())
	{
		if (m_PBI->isNull())
		{
			return false;
		}
		
		if (m_KeyIdIsDirty)
		{
			rebuildCache();
		}
	
		if (!m_IdSet.size())
		{
			return false;
		}
		
		for(int i(0), s(primitive.tagsSize()); i < s; ++i)
		{
			if (m_IdSet.count( primitive.keyId(i) ))
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		for(int i(0), s(primitive.tagsSize()); i < s; ++i)
		{
			if (m_KeySet.count( primitive.key(i) ))
			{
				return true;
			}
		}
		return false;
	}
}

AbstractTagFilter* MultiKeyTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	MultiKeyTagFilter * myCopy = new MultiKeyTagFilter(m_KeySet.begin(), m_KeySet.end());
	copies[this] = myCopy;
	return myCopy;
}

//MultiKeyValueTagFilter

MultiKeyMultiValueTagFilter::MultiKeyMultiValueTagFilter() {}

void MultiKeyMultiValueTagFilter::clearValues()
{
	m_ValueMap.clear();
}

bool MultiKeyMultiValueTagFilter::p_matches(const IPrimitive& primitive)
{
	for(int i(0), s(primitive.tagsSize()); i < s; ++i)
	{
		ValueMap::const_iterator it( m_ValueMap.find(primitive.key(i)) );
		if ( it != m_ValueMap.end() && it->second.count(primitive.value(i)) )
		{
			return true;
		}
	
	}
	return false;
}

void MultiKeyMultiValueTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor* ) {}

bool MultiKeyMultiValueTagFilter::rebuildCache() {return true;}

AbstractTagFilter* MultiKeyMultiValueTagFilter::copy(AbstractTagFilter::CopyMap & copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	MultiKeyMultiValueTagFilter * myCopy = new MultiKeyMultiValueTagFilter();
	for(const auto & x : m_ValueMap)
	{
		myCopy->addValues(x.first, x.second.cbegin(), x.second.cend());
	}
	copies[this] = myCopy;
	return myCopy;
}

RegexKeyTagFilter::RegexKeyTagFilter(std::regex_constants::match_flag_type flags) :
m_PBI(0),
m_matchFlags(flags),
m_dirty(true)
{}

RegexKeyTagFilter::RegexKeyTagFilter() :
RegexKeyTagFilter(std::regex_constants::match_default)
{}

RegexKeyTagFilter::RegexKeyTagFilter(const std::string & regexString, std::regex_constants::match_flag_type flags) :
m_PBI(0),
m_regex(regexString),
m_matchFlags(flags),
m_dirty(true)
{}

RegexKeyTagFilter::RegexKeyTagFilter(const std::regex & regex, std::regex_constants::match_flag_type flags) :
m_PBI(0),
m_regex(regex),
m_matchFlags(flags),
m_dirty(true)
{}

RegexKeyTagFilter::~RegexKeyTagFilter()
{}

void RegexKeyTagFilter::setRegex(const std::regex & regex, std::regex_constants::match_flag_type flags)
{
	m_regex = regex;
	m_matchFlags = flags;
	m_dirty = true;
}

void RegexKeyTagFilter::setRegex(const std::string & regexString, std::regex_constants::match_flag_type flags)
{
	m_regex.assign(regexString);
	m_matchFlags = flags;
	m_dirty = true;
}

// RegexKeyTagFilter
bool RegexKeyTagFilter::rebuildCache()
{
	m_IdSet.clear();
	m_dirty = false;

	if (!m_PBI)
	{
		return true;
	}
	
	if (m_PBI->isNull())
	{
		return false;
	}

	for (uint32_t id(0), s(m_PBI->stringTableSize()); id < s; ++id)
	{
		if (std::regex_match(m_PBI->queryStringTable(id), m_regex, m_matchFlags))
		{
			m_IdSet.insert(id);
		}
	}

	return m_IdSet.size();
}

void RegexKeyTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor* pbi)
{
	if (m_PBI != pbi)
	{
		m_PBI = pbi;
		m_dirty = true;
	}
}

AbstractTagFilter* RegexKeyTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	RegexKeyTagFilter * myCopy = new RegexKeyTagFilter(m_regex, m_matchFlags);
	copies[this] = myCopy;
	return myCopy;
}

bool RegexKeyTagFilter::p_matches(const IPrimitive& primitive)
{
	if (m_dirty || m_PBI != primitive.controller())
	{
		for(int i(0), s(primitive.tagsSize()); i < s; ++i)
		{
			if (std::regex_match(primitive.key(i), m_regex, m_matchFlags))
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		for(int i(0), s(primitive.tagsSize()); i < s; ++i)
		{
			if (m_IdSet.count(primitive.keyId(i)))
			{
				return true;
			}
		}
		return false;
	}
}

// BoolTagFilter

BoolTagFilter::BoolTagFilter(const std::string & key, bool value) :
	KeyMultiValueTagFilter(key),
	m_Value(value)
{
	if (m_Value)
	{
		const std::array<std::string, 5> s{ { "True", "true", "Yes", "yes", "1" } };
		KeyMultiValueTagFilter::setValues(s.begin(), s.end());
	}
	else
	{
		const std::array<std::string, 5> s{ { "False", "false", "No", "no", "0" } };
		KeyMultiValueTagFilter::setValues(s.begin(), s.end());
	}
}

bool BoolTagFilter::rebuildCache()
{
	return KeyMultiValueTagFilter::rebuildCache();
}

void BoolTagFilter::setValue(bool value)
{
	if (m_Value == value)
		return;

	m_Value = value;

	clearValues();

	if (m_Value)
	{
		const std::array<std::string, 5> s{ { "True", "true", "Yes", "yes", "1" } };
		KeyMultiValueTagFilter::setValues(s.begin(), s.end());
	}
	else
	{
		const std::array<std::string, 5> s{ { "False", "false", "No", "no", "0" } };
		KeyMultiValueTagFilter::setValues(s.begin(), s.end());
	}
}

bool BoolTagFilter::value() const
{
	return m_Value;
}

void BoolTagFilter::addValue(const std::string & value)
{
	KeyMultiValueTagFilter::addValue(value);
}

void BoolTagFilter::clearValues()
{
	KeyMultiValueTagFilter::clearValues();
}

KeyMultiValueTagFilter & BoolTagFilter::operator<<(const std::string & value)
{
	KeyMultiValueTagFilter::operator<<(value);
	return *this;
}

KeyMultiValueTagFilter & BoolTagFilter::operator<<(const char * value)
{
	KeyMultiValueTagFilter::operator<<(value);
	return *this;
}

AbstractTagFilter* BoolTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	BoolTagFilter * myCopy = new BoolTagFilter(key(), value());
	copies[this] = myCopy;
	return myCopy;
}


// IntTagFilter

IntTagFilter::IntTagFilter(const std::string & key, int value) :
KeyOnlyTagFilter(key), m_Value(value) {}

void IntTagFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor * pbi)
{
	if (m_PBI != pbi)
	{
		m_KeyIdIsDirty = true;
		m_ValueIdIsDirty = true;
	}
	m_PBI = pbi;
}

bool IntTagFilter::rebuildCache()
{
	m_KeyId = findId(m_Key);
	m_KeyIdIsDirty = false;

	findValueId();

	if (!m_PBI) return true;
	if (m_PBI->isNull()) return false;

	return m_KeyId && m_ValueId;
}

void IntTagFilter::setValue(int value)
{
	m_Value = value;
	findValueId();
}

int IntTagFilter::value() const
{
	return m_Value;
}

bool IntTagFilter::findValueId()
{
	m_ValueId = 0;
	m_ValueIdIsDirty = 0;

	if (!m_PBI)
		return true;

	uint32_t stringTableSize = m_PBI->stringTableSize();

	for (m_ValueId = 1; m_ValueId < stringTableSize; ++m_ValueId)
	{
		const std::string & tagValue = m_PBI->queryStringTable(m_ValueId);
		char * endptr;
		auto intTagValue = strtol(tagValue.c_str(), &endptr, 10);

		if ((*endptr == '\0') && (intTagValue == m_Value))
			break;
	}

	if (m_ValueId >= stringTableSize)
		m_ValueId = 0;

	return m_ValueId;
}

void IntTagFilter::checkValueIdCache()
{
	if (m_ValueIdIsDirty)
		findValueId();
}

bool IntTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;

	if (m_PBI == primitive.controller())
	{
		if (m_PBI->isNull())
			return false;

		checkKeyIdCache();
		checkValueIdCache();

		m_LatestMatch = findTag<IPrimitive>(primitive, m_KeyId, m_ValueId);
		return m_LatestMatch > -1;
	}

	m_LatestMatch = -1;

	for (int i = 0; i < primitive.tagsSize(); ++i)
	{
		if (primitive.key(i) == m_Key)
		{
			char * endptr;
			auto intTagValue = strtol(primitive.value(i).c_str(), &endptr, 10);

			if ((*endptr == '\0') && (intTagValue == m_Value))
			{
				m_LatestMatch = i;
				return true;
			}
		}
	}

	return false;
}

AbstractTagFilter* IntTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	IntTagFilter * myCopy = new IntTagFilter(key(), value());
	copies[this] = myCopy;
	return myCopy;
}

//InvertFilter

InversionFilter::InversionFilter() :
m_child(0)
{}

InversionFilter::InversionFilter(AbstractTagFilter* child) :
m_child(child)
{
	if (m_child)
	{
		m_child->rcInc();
	}
}

InversionFilter::~InversionFilter()
{
	if (m_child)
	{
		m_child->rcDec();
	}
}

void InversionFilter::assignInputAdaptor(const PrimitiveBlockInputAdaptor* pbi)
{
	if (m_child)
	{
		m_child->assignInputAdaptor(pbi);
	}
}

bool InversionFilter::rebuildCache()
{
	//if child matches something (not everything) then this matches something as-well
	//if child matches nothing, then this matches everything
	//but we still need to rebuild the cache of child nodes so that they have the correct cache info
	m_child->rebuildCache();
	return true;
}

void InversionFilter::setChild(AbstractTagFilter* child)
{
	child->rcInc();
	
	if (m_child)
	{
		m_child->rcDec();
	}
	
	m_child = child;
}

AbstractTagFilter * InversionFilter::child()
{
	return m_child;
}

const AbstractTagFilter * InversionFilter::child() const
{
	return m_child;
}

AbstractTagFilter* InversionFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	InversionFilter * myCopy = new InversionFilter();
	if (m_child)
	{
		myCopy->setChild( AbstractTagFilter::copy(m_child, copies) );
	}
	copies[this] = myCopy;
	return myCopy;
}

bool InversionFilter::p_matches(const IPrimitive& primitive)
{
	return (m_child? !m_child->matches(primitive) : true); //a null-child cannot match anything
}

void InversionFilter::invert(RCFilterPtr & filter)
{
	InversionFilter * tmp = dynamic_cast<InversionFilter*>(filter.get());
	if (tmp)
	{
		filter.reset(tmp->child());
	}
	else {
		filter.reset( new InversionFilter(filter.get()) );
	}
}

RCFilterPtr InversionFilter::invert(AbstractTagFilter * filter)
{
	RCFilterPtr tmp(filter);
	InversionFilter::invert(tmp);
	return tmp;
}

AndTagFilter * newAnd(AbstractTagFilter * a, AbstractTagFilter * b)
{
	AndTagFilter * result = new AndTagFilter();
	result->addChild(a);
	result->addChild(b);
	return result;
}

OrTagFilter * newOr(AbstractTagFilter * a, AbstractTagFilter * b)
{
	OrTagFilter * result = new OrTagFilter();
	result->addChild(a);
	result->addChild(b);
	return result;
}

} // namespace osmpbf
