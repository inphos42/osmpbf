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

#ifndef OSMPBF_NODESTREAMINPUTADAPTOR_H
#define OSMPBF_NODESTREAMINPUTADAPTOR_H

#include <osmpbf/common_input.h>
#include <osmpbf/abstractnodeinputadaptor.h>
#include <osmpbf/dataindex.h>

#include <cstdint>

namespace osmpbf
{

class NodeStreamInputAdaptor : public AbstractNodeInputAdaptor
{
public:
	NodeStreamInputAdaptor();
	NodeStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller);

	virtual bool isNull() const override;

	virtual int64_t id() override;

	virtual int tagsSize() const override;

	virtual uint32_t keyId(int index) const override;
	virtual uint32_t valueId(int index) const override;
	virtual bool hasInfo() const override;
	virtual IInfo info() const override;

	virtual int64_t lati() override;
	virtual int64_t loni() override;

	virtual double latd() override;
	virtual double lond() override;

	virtual int64_t rawLat() const override;
	virtual int64_t rawLon() const override;

	virtual NodeType nodeType() const override;

	void next();
	void previous();

private:
	void updateGroupMode();
	void updateWGS84();

	void nextNodeIndex();
	void previousNodeIndex();

	crosby::binary::PrimitiveGroup * getCurrentGroup() const;

	PrimitiveGroupVector::iterator m_PlainGroupIterator;
	DenseNodesDataVector::iterator m_DenseGroupIterator;

	NodeType m_GroupMode;

	int m_GroupNodeIndex;

	int64_t m_Id;
	int64_t m_Lat;
	int64_t m_Lon;
	int64_t m_WGS84Lat;
	int64_t m_WGS84Lon;
};

} // namespace osmpbf

#endif // OSMPBF_NODESTREAMINPUTADAPTOR_H
