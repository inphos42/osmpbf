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

#ifndef NODEDATACACHE_H
#define NODEDATACACHE_H

#include <osmpbf/pbf_prototypes.h>

#include <vector>

namespace osmpbf
{

class DenseNodesData
{
public:
	DenseNodesData() = delete;
	~DenseNodesData() = default;

	DenseNodesData(const DenseNodesData & other);

	explicit DenseNodesData(crosby::binary::PrimitiveGroup * denseNodesGroup, bool unpack);

	DenseNodesData & operator=(const DenseNodesData & other);

	inline crosby::binary::PrimitiveGroup * group() { return m_Group; }
	inline bool isDataUnpacked() const { return m_DataUnpacked; }

	inline int queryDenseNodeKeyValIndex(int index)
	{
		if (m_KeyValIndex.empty())
			buildDenseNodeKeyValIndex();

		return m_KeyValIndex[index];
	}

	void unpackData();

private:
	void buildDenseNodeKeyValIndex();

	crosby::binary::PrimitiveGroup * m_Group;
	std::vector<int> m_KeyValIndex;
	bool m_DataUnpacked = false;
};

typedef std::vector<crosby::binary::PrimitiveGroup *> PrimitiveGroupVector;
typedef std::vector<DenseNodesData> DenseNodesDataVector;

} // namespace osmpbf

#endif // NODEDATACACHE_H
