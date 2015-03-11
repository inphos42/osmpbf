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

#include <generics/store.h>

#include "osmformat.pb.h"

#include <osmpbf/abstractprimitiveoutputadaptor.h>

namespace osmpbf {
	template class AbstractPrimitiveOutputAdaptor< crosby::binary::Way >;
	template class AbstractPrimitiveOutputAdaptor< crosby::binary::Node >;
	template class AbstractPrimitiveOutputAdaptor< crosby::binary::Relation >;
}

#include <osmpbf/common.h>

namespace osmpbf {
	template<class PrimitiveType>
	int64_t AbstractPrimitiveOutputAdaptor<PrimitiveType>::id() const {
		return m_Data->id();
	}

	template<class PrimitiveType>
	void AbstractPrimitiveOutputAdaptor<PrimitiveType>::setId(int64_t value) {
		m_Data->set_id(value);
	}

	template<class PrimitiveType>
	int AbstractPrimitiveOutputAdaptor<PrimitiveType>::tagsSize() const {
		return m_Data->keys_size();
	}

	template<class PrimitiveType>
	const std::string & AbstractPrimitiveOutputAdaptor<PrimitiveType>::key(int index) const {
		return m_StringTable->query(m_Data->keys(index));
	}

	template<class PrimitiveType>
	const std::string & AbstractPrimitiveOutputAdaptor<PrimitiveType>::value(int index) const {
		return m_StringTable->query(m_Data->vals(index));
	}

	template<class PrimitiveType>
	void AbstractPrimitiveOutputAdaptor<PrimitiveType>::setKey(int index, const std::string & key) {
		m_Data->set_keys(index, m_StringTable->insert(key));
	}

	template<class PrimitiveType>
	void AbstractPrimitiveOutputAdaptor<PrimitiveType>::setValue(int index, const std::string & value) {
		m_Data->set_vals(index, m_StringTable->insert(value));
	}

	template<class PrimitiveType>
	void AbstractPrimitiveOutputAdaptor<PrimitiveType>::setValue(const std::string & key, const std::string & value) {
		if (!m_Data->keys_size())
			return;

		uint32_t keyId = m_StringTable->id(key);

		for (int i = 0; i < m_Data->keys_size(); ++i) {
			if (m_Data->keys(i) == keyId)
				setValue(i, value);
		}
	}

	template<class PrimitiveType>
	void AbstractPrimitiveOutputAdaptor<PrimitiveType>::addTag(const std::string & key, const std::string & value) {
		m_Data->add_keys(m_StringTable->insert(key));
		m_Data->add_vals(m_StringTable->insert(value));
	}

	template<class PrimitiveType>
	void AbstractPrimitiveOutputAdaptor<PrimitiveType>::removeTagLater(int index) {
		m_StringTable->remove(m_Data->keys(index));
		m_StringTable->remove(m_Data->vals(index));

		m_Data->set_keys(index, NULL_STRING_ID);
		m_Data->set_vals(index, NULL_STRING_ID);
	}

	template<class PrimitiveType>
	void AbstractPrimitiveOutputAdaptor<PrimitiveType>::clearTags() {
		for (int i = 0; i < m_Data->keys_size(); ++i)
			m_StringTable->remove(m_Data->keys(i));

		for (int i = 0; i < m_Data->keys_size(); ++i)
			m_StringTable->remove(m_Data->vals(i));

		m_Data->clear_keys();
		m_Data->clear_vals();
	}
}
