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

#ifndef OSMPBF_IPRIMITIVE
#define OSMPBF_IPRIMITIVE

#include <osmpbf/common_input.h>
#include <osmpbf/abstractprimitiveinputadaptor.h>
#include <osmpbf/iinfo.h>

#include <generics/refcountobject.h>

namespace osmpbf
{

class IPrimitive : public generics::RCWrapper< AbstractPrimitiveInputAdaptor >
{
public:
	IPrimitive(const IPrimitive & other) : RCWrapper< AbstractPrimitiveInputAdaptor >(other) {}

	inline virtual bool isNull() const { return generics::RCWrapper< AbstractPrimitiveInputAdaptor >::isNull() || m_Private->isNull(); }

	inline IPrimitive & operator=(const IPrimitive & other) { RCWrapper< AbstractPrimitiveInputAdaptor >::operator=(other); return *this; }

	inline int64_t id() const { return m_Private->id(); }
	inline osmpbf::PrimitiveType type() const { return m_Private->type(); }

	inline int tagsSize() const { return m_Private->tagsSize(); }

	inline uint32_t keyId(int index) const { return m_Private->keyId(index); }
	inline uint32_t valueId(int index) const { return m_Private->valueId(index); }
	
	inline bool hasInfo() const { return m_Private->hasInfo(); }
	inline IInfo info() const { return m_Private->info(); }

	inline const std::string & key(int index) const { return m_Private->key(index); }
	inline const std::string & value(int index) const { return m_Private->value(index); }

	inline const std::string & valueByKeyId(uint32_t key) const { return m_Private->valueByKeyId(key); }
	inline const std::string & valueByKey(const std::string key) const { return m_Private->valueByKey(key); }

protected:
	IPrimitive() : RCWrapper< AbstractPrimitiveInputAdaptor >() {}
	IPrimitive (AbstractPrimitiveInputAdaptor * data) : RCWrapper< AbstractPrimitiveInputAdaptor >(data) {}
};

} // namespace osmpbf

#endif // OSMPBF_IPRIMITIVE
