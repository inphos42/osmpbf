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

#ifndef OSMPBF_ABSTRACTPRIMITIVEINPUTADAPTOR_H
#define OSMPBF_ABSTRACTPRIMITIVEINPUTADAPTOR_H

#include <cstdint>
#include <string>

#include "typelimits.h"

#include <generics/refcountobject.h>

namespace osmpbf {
	class PrimitiveBlockInputAdaptor;

	class AbstractPrimitiveInputAdaptor : public generics::RefCountObject {
	public:
		AbstractPrimitiveInputAdaptor()
			: RefCountObject(), m_Controller(NULL) {}
		AbstractPrimitiveInputAdaptor(PrimitiveBlockInputAdaptor * controller)
			: RefCountObject(), m_Controller(controller) {}

		virtual bool isNull() const { return !m_Controller; }

		virtual int64_t id() = 0;

		virtual int tagsSize() const = 0;

		virtual uint32_t keyId(int index) const = 0;
		virtual uint32_t valueId(int index) const = 0;

		virtual const std::string & key(int index) const;
		virtual const std::string & value(int index) const;

		// convenience functions (very slow)
		virtual const std::string & valueByKeyId(uint32_t key) const;
		virtual const std::string & valueByKey(const std::string & key) const;

	protected:
		PrimitiveBlockInputAdaptor * m_Controller;
	};
}

#endif // OSMPBF_ABSTRACTPRIMITIVEADAPTOR_H
