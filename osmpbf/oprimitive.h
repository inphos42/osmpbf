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

#ifndef OSMPBF_OPRIMITIVE_H
#define OSMPBF_OPRIMITIVE_H

#include <generics/refcountobject.h>

namespace osmpbf {

	template < class OutputAdaptor >
	class OPrimitive : public generics::RCWrapper< OutputAdaptor > {
	public:
		OPrimitive(const OPrimitive & other) : generics::RCWrapper< OutputAdaptor >(other) {}

		inline virtual bool isNull() const { return generics::RCWrapper< OutputAdaptor >::isNull() || generics::RCWrapper< OutputAdaptor >::m_Private->isNull(); }

		inline OPrimitive & operator=(const OPrimitive & other) { generics::RCWrapper< OutputAdaptor >::operator=(other); return *this; }

		inline int64_t id() const { return generics::RCWrapper< OutputAdaptor >::m_Private->id(); }
		inline void setId(int64_t value) { generics::RCWrapper< OutputAdaptor >::m_Private->setId(value); }

		inline int tagsSize() const { return generics::RCWrapper< OutputAdaptor >::m_Private->tagsSize(); }

		inline const std::string & key(int index) { return generics::RCWrapper< OutputAdaptor >::m_Private->key(index); }
		inline const std::string & value(int index) { return generics::RCWrapper< OutputAdaptor >::m_Private->value(index); }

		inline void addTag(const std::string & key, const std::string & value) { generics::RCWrapper< OutputAdaptor >::m_Private->addTag(key, value); }
		inline void removeTagLater(int index) { generics::RCWrapper< OutputAdaptor >::m_Private->removeTagLater(index); }

		inline void clearTags() { generics::RCWrapper< OutputAdaptor >::m_Private->clearTags(); }

	protected:
		OPrimitive() : generics::RCWrapper< OutputAdaptor >() {}
		OPrimitive(OutputAdaptor * data) : generics::RCWrapper< OutputAdaptor >(data) {}
	};

}

#endif // OSMPBF_OPRIMITIVE_H
