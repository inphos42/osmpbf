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

	virtual bool isNull() const;

	virtual int64_t id() { return m_Id; }

	virtual int tagsSize() const;

	virtual uint32_t keyId(int index) const;
	virtual uint32_t valueId(int index) const;

	virtual int64_t lati() { return m_WGS84Lat; }
	virtual int64_t loni() { return m_WGS84Lon; }

	virtual double latd() { return m_WGS84Lat * .000000001; }
	virtual double lond() { return m_WGS84Lon * .000000001; }

	virtual int64_t rawLat() const { return m_Lat; }
	virtual int64_t rawLon() const { return m_Lon; }

	virtual NodeType nodeType() const { return m_GroupMode; }

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
