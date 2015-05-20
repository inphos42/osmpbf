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

#ifndef OSMPBF_RELATIONINPUTADAPTOR_H
#define OSMPBF_RELATIONINPUTADAPTOR_H

#include <osmpbf/common_input.h>
#include <osmpbf/pbf_prototypes.h>
#include <osmpbf/abstractprimitiveinputadaptor.h>
#include <osmpbf/dataindex.h>

#include <generics/refcountobject.h>

#include <cstdint>

namespace osmpbf
{

class MemberStreamInputAdaptor : public generics::RefCountObject
{
public:
	MemberStreamInputAdaptor();
	explicit MemberStreamInputAdaptor(const crosby::binary::Relation * data);

	bool isNull() const;

	int64_t id() const;
	PrimitiveType type() const;
	uint32_t roleId() const;

	void next();
	void previous();

protected:
	const crosby::binary::Relation * m_Data;

	int m_Index;
	const int m_MaxIndex;

	int64_t m_CachedId;
};

class IMemberStream : public generics::RCWrapper< MemberStreamInputAdaptor >
{
public:
	IMemberStream(const IMemberStream & other);

	IMemberStream & operator=(const IMemberStream & other);

	virtual bool isNull() const;

	inline int64_t id() const { return m_Private->id(); }

	inline osmpbf::PrimitiveType type() const { return m_Private->type(); }

	inline uint32_t roleId() const { return m_Private->roleId(); }

	const std::string & role() const;

	inline void next() const { m_Private->next(); }

protected:
	friend class RelationInputAdaptor;

	IMemberStream();
	explicit IMemberStream(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Relation * data);

	PrimitiveBlockInputAdaptor * m_Controller;
};

class RelationInputAdaptor : public AbstractPrimitiveInputAdaptor
{
public:
	RelationInputAdaptor();
	explicit RelationInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Relation * data);

	virtual bool isNull() const override;

	virtual int64_t id() override;
	virtual osmpbf::PrimitiveType type() const override;

	virtual int tagsSize() const override;

	virtual uint32_t keyId(int index) const override;
	virtual uint32_t valueId(int index) const override;
	
	virtual bool hasInfo() const override;
	virtual IInfo info() const override;
	
	virtual int membersSize() const;

	virtual IMemberStream getMemberStream() const;

protected:
	const crosby::binary::Relation * m_Data;
};

class RelationStreamInputAdaptor : public RelationInputAdaptor
{
public:
	RelationStreamInputAdaptor();
	RelationStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller);

	virtual bool isNull() const override;

	void next();
	void previous();

private:
	crosby::binary::PrimitiveGroup * getCurrentGroup() const;

	int m_Index;

	PrimitiveGroupVector::iterator m_GroupIterator;
};

} // namespace osmpbf

#endif // OSMPBF_RELATIONINPUTADAPTOR_H
