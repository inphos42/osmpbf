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

#ifndef OSMPBF_ABSTRACTPRIMITIVEOUTPUTADAPTOR_H
#define OSMPBF_ABSTRACTPRIMITIVEOUTPUTADAPTOR_H

#include <osmpbf/stringtable_fwd.h>

#include <generics/refcountobject.h>

#include <cstdint>
#include <string>

namespace osmpbf {

	template<class PrimitiveType>
	class AbstractPrimitiveOutputAdaptor : public generics::RefCountObject {
	public:
		AbstractPrimitiveOutputAdaptor()
			: generics::RefCountObject(), m_StringTable(NULL), m_Data(NULL) {}
		AbstractPrimitiveOutputAdaptor(StringTable * stringTable, PrimitiveType * data)
			: generics::RefCountObject(), m_StringTable(stringTable), m_Data(data) {}

		virtual bool isNull() const { return !m_StringTable || !m_Data; }

		virtual int64_t id() const;

		virtual void setId(int64_t value);

		virtual int tagsSize() const;

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;

		virtual void setKey(int index, const std::string & key);
		virtual void setValue(int index, const std::string & value);
		virtual void setValue(const std::string & key, const std::string & value);

		virtual void addTag(const std::string & key, const std::string & value);
		virtual void removeTagLater(int index);

		virtual void clearTags();

	protected:
		StringTable * m_StringTable;
		PrimitiveType * m_Data;
	};
}

#endif // OSMPBF_ABSTRACTPRIMITIVEOUTPUTADAPTOR_H
