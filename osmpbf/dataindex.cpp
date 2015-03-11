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

#include "osmpbf/dataindex.h"

#include "osmformat.pb.h"

#include <cstddef>
#include <iostream>
#include <assert.h>

namespace osmpbf
{

// DenseNodesData

DenseNodesData::DenseNodesData(const DenseNodesData & other) : m_Group(other.m_Group), m_KeyValIndex(other.m_KeyValIndex) {}

DenseNodesData::DenseNodesData(crosby::binary::PrimitiveGroup * denseNodesGroup, bool unpack)
	: m_Group(denseNodesGroup)
{
	if (unpack)
		unpackData();
}

DenseNodesData & DenseNodesData::operator=(const DenseNodesData & other)
{
	m_Group = other.m_Group;
	m_KeyValIndex = other.m_KeyValIndex;

	return *this;
}

void DenseNodesData::buildDenseNodeKeyValIndex()
{
	int keys_vals_size = m_Group->dense().keys_vals_size();

	if (!keys_vals_size)
		return;

	m_KeyValIndex.resize(m_Group->dense().id_size() * 2, 0);

	int keyValPos = 0;
	int keyValLength = 0;

	int i = 0;
	while(i < keys_vals_size) {
		if (m_Group->dense().keys_vals(i)) {
			keyValLength++;
			i++;
		}
		else {
			m_KeyValIndex[keyValPos * 2] = i - (keyValLength * 2);
			m_KeyValIndex[keyValPos * 2 + 1] = keyValLength;
			keyValPos++;
			keyValLength = 0;
		}

		i++;
	}

	assert(keyValPos == m_Group->dense().id_size());
}

void DenseNodesData::unpackData()
{
	if (!m_Group || m_DataUnpacked)
		return;

	m_DataUnpacked = true;

	int64_t id  = m_Group->dense().id(0);
	int64_t lat = m_Group->dense().lat(0);
	int64_t lon = m_Group->dense().lon(0);

	for (int i = 1; i < m_Group->dense().id_size(); i++) {
		id  += m_Group->dense().id(i);
		lat += m_Group->dense().lat(i);
		lon += m_Group->dense().lon(i);

		m_Group->mutable_dense()->mutable_id()->Set(i, id);
		m_Group->mutable_dense()->mutable_lat()->Set(i, lat);
		m_Group->mutable_dense()->mutable_lon()->Set(i, lon);
	}
}

} // namespace osmpbf
