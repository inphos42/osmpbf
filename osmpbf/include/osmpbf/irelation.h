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

#ifndef OSMPBF_IRELATION_H
#define OSMPBF_IRELATION_H

#include <osmpbf/common_input.h>
#include <osmpbf/iprimitive.h>
#include <osmpbf/relationinputadaptor.h>

namespace osmpbf
{

class IRelation : public IPrimitive
{
public:
	typedef IMemberStream MemberStream;
public:
	explicit IRelation(RelationInputAdaptor * data);
	IRelation(const IRelation & other);

	IRelation & operator=(const IRelation & other);

	inline int membersSize() const { return static_cast< RelationInputAdaptor * >(m_Private)->membersSize(); }

	inline MemberStream getMemberStream() const { return static_cast< RelationInputAdaptor * >(m_Private)->getMemberStream(); }

protected:
	IRelation();
};


class IRelationStream : public IRelation
{
public:
	explicit IRelationStream(PrimitiveBlockInputAdaptor * controller);
	IRelationStream(const IRelationStream & other);

	IRelationStream & operator=(const IRelationStream & other);

	inline void next() { static_cast<RelationStreamInputAdaptor *>(m_Private)->next(); }
	inline void previous() { static_cast<RelationStreamInputAdaptor *>(m_Private)->previous(); }

protected:
	IRelationStream();
	IRelationStream(RelationInputAdaptor * data);
};

} // namespace osmpbf

#endif // OSMPBF_IRELATION_H
