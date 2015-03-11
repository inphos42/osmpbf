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

#ifndef OSMPBF_OWAY_H
#define OSMPBF_OWAY_H

#include <osmpbf/common.h>
#include <osmpbf/abstractprimitiveoutputadaptor.h>
#include <osmpbf/oprimitive.h>

#include <cstdint>
#include <string>
#include <utility>

#include <generics/fielditerator.h>

namespace crosby {
	namespace binary {
		class Way;
	}
}

namespace osmpbf {
	class PrimitiveBlockOutputAdaptor;

	class WayOutputAdaptor : public AbstractPrimitiveOutputAdaptor< crosby::binary::Way > {
	public:
		WayOutputAdaptor();
		WayOutputAdaptor(PrimitiveBlockOutputAdaptor * controller, crosby::binary::Way * data);

		virtual int refsSize() const;

		virtual int64_t ref(int index) const;
		virtual void setRef(int index, int64_t value);

		virtual void addRef(int64_t ref);

		virtual void setRefs(const generics::DeltaFieldConstForwardIterator<int64_t> & from, const generics::DeltaFieldConstForwardIterator<int64_t> & to);
		virtual void setRefs(const generics::FieldConstIterator<int64_t> & from, const generics::FieldConstIterator<int64_t> & to);

		virtual void clearRefs();
	};

	class OWay : public OPrimitive< WayOutputAdaptor > {
		friend class PrimitiveBlockOutputAdaptor;
	public:
		OWay(const OWay & other);

		OWay & operator=(const OWay & other);

		inline int refsSize() const { return m_Private->refsSize(); }

		inline int64_t ref(int index) const { return m_Private->ref(index); }
		inline void setRef(int index, int64_t value) { m_Private->setRef(index, value); }

		inline void addRef(int64_t ref) { m_Private->addRef(ref); }
		inline void removeRefLater(int index) { m_Private->setRef(index, NULL_PRIMITIVE_ID); }

		inline void setRefs(const generics::DeltaFieldConstForwardIterator<int64_t> & from, const generics::DeltaFieldConstForwardIterator<int64_t> & to) {
			m_Private->setRefs(from, to); }
		inline void setRefs(const generics::FieldConstIterator<int64_t> & from, const generics::FieldConstIterator<int64_t> & to) {
			m_Private->setRefs(from, to); }

		virtual void clearRefs();

	protected:
		OWay();
		OWay(WayOutputAdaptor * data);
	};
}

#endif // OSMPBF_OWAY_H
