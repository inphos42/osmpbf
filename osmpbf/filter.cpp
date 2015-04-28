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

#include "osmpbf/filter.h"

#include <osmpbf/primitiveblockinputadaptor.h>
#include <osmpbf/iprimitive.h>
#include <osmpbf/irelation.h>

#include <cstdint>
#include <cstdlib>

namespace osmpbf
{

AbstractTagFilter* AbstractTagFilter::copy() const
{
	AbstractTagFilter::CopyMap cm;
	return this->copy(cm);
}

//PrimitiveTypeFilter
PrimitiveTypeFilter::PrimitiveTypeFilter(PrimitiveTypeFlags primitiveTypes) : m_filteredPrimitives(primitiveTypes) {}

PrimitiveTypeFilter::~PrimitiveTypeFilter() {}

void PrimitiveTypeFilter::setFilteredTypes(PrimitiveTypeFlags primitiveTypes)
{
	m_filteredPrimitives = primitiveTypes;
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
	myCopy->m_Invert = this->m_Invert;
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

// OrTagFilter

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
	myCopy->m_Invert = m_Invert;
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
	myCopy->m_Invert = m_Invert;
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

bool KeyOnlyTagFilter::rebuildCache()
{
	m_KeyId = findId(m_Key);
	m_KeyIdIsDirty = false;

	if (!m_PBI) return true;
	if (m_PBI->isNull()) return false;

	return  m_KeyId;
}

void KeyOnlyTagFilter::setKey(const std::string & key)
{
	m_Key = key;
	m_KeyIdIsDirty = true;
}

bool KeyOnlyTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;

	if (m_PBI)
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

AbstractTagFilter* KeyOnlyTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	KeyOnlyTagFilter * myCopy = new KeyOnlyTagFilter(key());
	myCopy->m_Invert = this->m_Invert;
	copies[this] = myCopy;
	return myCopy;
}


// StringTagFilter

StringTagFilter::StringTagFilter(const std::string & key, const std::string & value) :
	KeyOnlyTagFilter(key), m_Value(value), m_ValueId(0), m_ValueIdIsDirty(false) {}

bool StringTagFilter::rebuildCache()
{
	m_KeyId = findId(m_Key);
	m_KeyIdIsDirty = false;

	m_ValueId = findId(m_Value);
	m_ValueIdIsDirty = false;

	if (!m_PBI) return true;
	if (m_PBI->isNull()) return false;

	return m_KeyId && m_ValueId;
}

void StringTagFilter::setValue(const std::string & value)
{
	m_Value = value;
	m_ValueIdIsDirty = true;
}

bool StringTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;

	if (m_PBI)
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

AbstractTagFilter* StringTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	StringTagFilter * myCopy = new StringTagFilter(key(), value());
	myCopy->m_Invert = this->m_Invert;
	copies[this] = myCopy;
	return myCopy;
}

// MultiStringTagFilter

MultiStringTagFilter::MultiStringTagFilter(const std::string & key) : KeyOnlyTagFilter(key) {}

MultiStringTagFilter::MultiStringTagFilter(const std::string & key, std::initializer_list<std::string> l) :
	KeyOnlyTagFilter(key),
	m_ValueSet(l)
{
	updateValueIds();
}

bool MultiStringTagFilter::rebuildCache()
{
	m_KeyId = findId(m_Key);
	m_KeyIdIsDirty = false;

	updateValueIds();

	if (!m_PBI) return true;
	if (m_PBI->isNull()) return false;

	return m_KeyId && m_IdSet.size();
}

void MultiStringTagFilter::addValue(const std::string & value)
{
	m_ValueSet.insert(value);

	uint32_t valueId = findId(value);

	if (valueId)
		m_IdSet.insert(valueId);
}

void MultiStringTagFilter::clearValues()
{
	m_IdSet.clear();
	m_ValueSet.clear();
}

bool MultiStringTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;

	m_LatestMatch = -1;

	if (m_PBI)
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

void MultiStringTagFilter::updateValueIds()
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

AbstractTagFilter* MultiStringTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	MultiStringTagFilter * myCopy = new MultiStringTagFilter(key(), m_ValueSet.begin(), m_ValueSet.end());
	myCopy->m_Invert = this->m_Invert;
	copies[this] = myCopy;
	return myCopy;
}

//MultiKeyTagFilter
MultiKeyTagFilter::MultiKeyTagFilter(std::initializer_list< std::string > l) :
m_PBI(0),
m_KeyIdIsDirty(true),
m_ValueSet(l.begin(), l.end())
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
		if (m_ValueSet.count(m_PBI->queryStringTable(i)))
		{
			m_IdSet.insert(i);
		}
	}
	return m_IdSet.size();
}

void MultiKeyTagFilter::addValue(const std::string& value)
{
	m_ValueSet.insert(value);
	m_KeyIdIsDirty = true;
}

void MultiKeyTagFilter::clearValues()
{
	m_IdSet.clear();
	m_ValueSet.clear();
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
	if (m_ValueSet.empty())
	{
		return false;
	}
	
	if (m_PBI)
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
			if (m_ValueSet.count( primitive.key(i) ))
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
	MultiKeyTagFilter * myCopy = new MultiKeyTagFilter(m_ValueSet.begin(), m_ValueSet.end());
	myCopy->m_Invert = m_Invert;
	copies[this] = myCopy;
	return myCopy;
}

// BoolTagFilter

BoolTagFilter::BoolTagFilter(const std::string & key, bool value) :
	MultiStringTagFilter(key),
	m_Value(value)
{
	if (m_Value)
	{
		MultiStringTagFilter::setValues({"True", "true", "Yes", "yes", "1"});
	}
	else
	{
		MultiStringTagFilter::setValues({"False", "false", "No", "no", "0"});
	}
}

bool BoolTagFilter::rebuildCache()
{
	return MultiStringTagFilter::rebuildCache();
}

void BoolTagFilter::setValue(bool value)
{
	if (m_Value == value)
		return;

	m_Value = value;

	clearValues();

	if (m_Value)
	{
		MultiStringTagFilter::setValues({"True", "true", "Yes", "yes", "1"});
	}
	else
	{
		MultiStringTagFilter::setValues({"False", "false", "No", "no", "0"});
	}
}

AbstractTagFilter* BoolTagFilter::copy(AbstractTagFilter::CopyMap& copies) const
{
	if (copies.count(this))
	{
		return copies.at(this);
	}
	BoolTagFilter * myCopy = new BoolTagFilter(key(), value());
	myCopy->m_Invert = this->m_Invert;
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
		int intTagValue = strtol(tagValue.c_str(), &endptr, 10);

		if ((*endptr == '\0') && (intTagValue == m_Value))
			break;
	}

	if (m_ValueId >= stringTableSize)
		m_ValueId = 0;

	return m_ValueId;
}

bool IntTagFilter::p_matches(const IPrimitive & primitive)
{
	if (m_Key.empty())
		return false;

	if (m_PBI)
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
			int intTagValue = strtol(primitive.value(i).c_str(), &endptr, 10);

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
	myCopy->m_Invert = this->m_Invert;
	copies[this] = myCopy;
	return myCopy;
}


} // namespace osmpbf
