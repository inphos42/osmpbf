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

#ifndef OSMPBF_WAYINPUTADAPTOR_H
#define OSMPBF_WAYINPUTADAPTOR_H

#include <osmpbf/common_input.h>
#include <osmpbf/pbf_prototypes.h>
#include <osmpbf/abstractprimitiveinputadaptor.h>
#include <osmpbf/dataindex.h>

#include <cstdint>
#include <string>

namespace osmpbf
{

class WayInputAdaptor : public AbstractPrimitiveInputAdaptor
{
public:
	WayInputAdaptor();
	WayInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Way * data);

	virtual bool isNull() const override;

	virtual int64_t id() override;
	virtual osmpbf::PrimitiveType type() const override;

	virtual int tagsSize() const override;

	virtual uint32_t keyId(int index) const override;
	virtual uint32_t valueId(int index) const override;
	
	virtual bool hasInfo() const override;
	virtual IInfo info() const override;

	int refsSize() const;
	int64_t rawRef(int index) const;

	/// warning: This methods complexity is O(n). It's here for convenience. You shouldn't
	///          call this method very often or with a high index parameter.
	int64_t ref(int index) const;

	RefIterator refBegin() const;
	RefIterator refEnd() const;

protected:
	const crosby::binary::Way * m_Data;
};


class WayStreamInputAdaptor : public WayInputAdaptor
{
public:
	WayStreamInputAdaptor();
	WayStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller);

	virtual bool isNull() const override;

	void next();
	void previous();

private:
	crosby::binary::PrimitiveGroup * getCurrentGroup() const;

	int m_Index;

	PrimitiveGroupVector::iterator m_GroupIterator;
};

} // namespace osmpbf

#endif // OSMPBF_WAYINPUTADAPTOR_H
