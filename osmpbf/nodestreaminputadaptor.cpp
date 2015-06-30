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

#include "osmpbf/nodestreaminputadaptor.h"

#include "osmformat.pb.h"

#include <osmpbf/primitiveblockinputadaptor.h>

namespace osmpbf
{

NodeStreamInputAdaptor::NodeStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller) :
	AbstractNodeInputAdaptor(controller),
	m_GroupMode(NodeType::Undefined),
	m_GroupNodeIndex(-1),
	m_Id(0),
	m_Lat(0), m_Lon(0),
	m_WGS84Lat(0), m_WGS84Lon(0)
{
	if (m_Controller)
	{
		m_PlainGroupIterator = controller->m_PlainNodesGroups.begin();
		m_DenseGroupIterator = controller->m_DenseNodesGroups.begin();
	}

	updateGroupMode();

	next();
}

bool NodeStreamInputAdaptor::isNull() const
{
	return AbstractPrimitiveInputAdaptor::isNull() ||
		m_GroupMode == NodeType::Undefined ||
		(m_PlainGroupIterator == m_Controller->m_PlainNodesGroups.end() &&
		 m_DenseGroupIterator == m_Controller->m_DenseNodesGroups.end());
}

int64_t NodeStreamInputAdaptor::id()
{
	return m_Id;
}

void NodeStreamInputAdaptor::next()
{
	if (AbstractPrimitiveInputAdaptor::isNull())
		return;

	nextNodeIndex();

	const crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
		return;

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		m_Id = group->nodes(m_GroupNodeIndex).id();
		m_Lat = group->nodes(m_GroupNodeIndex).lat();
		m_Lon = group->nodes(m_GroupNodeIndex).lon();
		break;
	case NodeType::DenseNode:
		if (m_DenseGroupIterator->isDataUnpacked() || m_GroupNodeIndex == 0)
		{
			m_Id = group->dense().id(m_GroupNodeIndex);
			m_Lat = group->dense().lat(m_GroupNodeIndex);
			m_Lon = group->dense().lon(m_GroupNodeIndex);
		}
		else
		{
			m_Id += group->dense().id(m_GroupNodeIndex);
			m_Lat += group->dense().lat(m_GroupNodeIndex);
			m_Lon += group->dense().lon(m_GroupNodeIndex);
		}
		break;
	case NodeType::Undefined:
	default:
		return;
	}

	updateWGS84();
}

void NodeStreamInputAdaptor::previous()
{
	if (AbstractPrimitiveInputAdaptor::isNull())
		return;

	previousNodeIndex();

	crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
		return;

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		m_Id = group->nodes(m_GroupNodeIndex).id();
		m_Lat = group->nodes(m_GroupNodeIndex).lat();
		m_Lon = group->nodes(m_GroupNodeIndex).lon();
		break;
	case NodeType::DenseNode:
		if (m_DenseGroupIterator->isDataUnpacked() || m_GroupNodeIndex == 0)
		{
			m_Id = group->dense().id(m_GroupNodeIndex);
			m_Lat = group->dense().lat(m_GroupNodeIndex);
			m_Lon = group->dense().lon(m_GroupNodeIndex);
		}
		else
		{
			m_Id -= group->dense().id(m_GroupNodeIndex + 1);
			m_Lat -= group->dense().lat(m_GroupNodeIndex + 1);
			m_Lon -= group->dense().lon(m_GroupNodeIndex + 1);
		}
		break;
	case NodeType::Undefined:
	default:
		m_Id = 0;
		m_Lat = 0;
		m_Lon = 0;
		break;
	}

	updateWGS84();
}

int NodeStreamInputAdaptor::tagsSize() const
{
	crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
		return 0;

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		return group->nodes(m_GroupNodeIndex).keys_size();
	case NodeType::DenseNode:
		return group->dense().keys_vals_size() ?
			m_DenseGroupIterator->queryDenseNodeKeyValIndex(m_GroupNodeIndex * 2 + 1) : 0;
	case NodeType::Undefined:
	default:
		return 0;
	}
}

uint32_t NodeStreamInputAdaptor::keyId(int index) const
{
	if (index < 0 || index > tagsSize())
		return NULL_STRING_ID;

	crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
		return NULL_STRING_ID;

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		return group->nodes(m_GroupNodeIndex).keys(index);
	case NodeType::DenseNode:
		{
			const int keyValIndex = m_DenseGroupIterator->queryDenseNodeKeyValIndex(m_GroupNodeIndex * 2) + index * 2;

			return group->dense().keys_vals_size() ? group->dense().keys_vals(keyValIndex) : 0;
		}
	case NodeType::Undefined:
	default:
		return NULL_STRING_ID;
	}
}

uint32_t NodeStreamInputAdaptor::valueId(int index) const
{
	if (index < 0 || index > tagsSize())
		return NULL_STRING_ID;

	const crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
		return NULL_STRING_ID;

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		return group->nodes(m_GroupNodeIndex).vals(index);
	case NodeType::DenseNode:
		{
			const int keyValIndex = m_DenseGroupIterator->queryDenseNodeKeyValIndex(m_GroupNodeIndex * 2) + index * 2 + 1;

			return group->dense().keys_vals_size() ? group->dense().keys_vals(keyValIndex) : 0;
		}
	case NodeType::Undefined:
	default:
		return NULL_STRING_ID;
	}
}

bool NodeStreamInputAdaptor::hasInfo() const
{
	const crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
	{
		return false;
	}
	
	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		return group->nodes(m_GroupNodeIndex).has_info();
	case NodeType::DenseNode: //deal with this later
	case NodeType::Undefined:
	default:
		return false;
	}
}

IInfo NodeStreamInputAdaptor::info() const
{
	const crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
	{
		return IInfo();
	}
	
	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		if (group->nodes(m_GroupNodeIndex).has_info()) {
			return IInfo(group->nodes(m_GroupNodeIndex).info());
		}
		return IInfo();
	case NodeType::DenseNode: //deal with this later
	case NodeType::Undefined:
	default:
		return IInfo();
	}
}


int64_t NodeStreamInputAdaptor::lati()
{
	return m_WGS84Lat;
}

int64_t NodeStreamInputAdaptor::loni()
{
	return m_WGS84Lon;
}

double NodeStreamInputAdaptor::latd()
{
	return m_WGS84Lat * COORDINATE_SCALE_FACTOR_LAT;
}

double NodeStreamInputAdaptor::lond()
{
	return m_WGS84Lon * COORDINATE_SCALE_FACTOR_LON;
}

int64_t NodeStreamInputAdaptor::rawLat() const
{
	return m_Lat;
}

int64_t NodeStreamInputAdaptor::rawLon() const
{
	return m_Lon;
}

NodeType NodeStreamInputAdaptor::nodeType() const
{
	return m_GroupMode;
}

void NodeStreamInputAdaptor::updateGroupMode()
{
	m_GroupMode = NodeType::Undefined;

	if (m_Controller == nullptr)
		return;

	if (m_PlainGroupIterator != m_Controller->m_PlainNodesGroups.end())
	{
		m_GroupMode = NodeType::PlainNode;
	}
	else if (m_DenseGroupIterator != m_Controller->m_DenseNodesGroups.end())
	{
		m_GroupMode = NodeType::DenseNode;
	}
}

void NodeStreamInputAdaptor::updateWGS84()
{
	m_WGS84Lat = m_Controller->toWGS84Lati(m_Lat);
	m_WGS84Lon = m_Controller->toWGS84Loni(m_Lon);
}

void NodeStreamInputAdaptor::nextNodeIndex()
{
	const crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
	{
		m_GroupNodeIndex = 0;
		updateGroupMode();
		return;
	}

	m_GroupNodeIndex++;

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		if (m_GroupNodeIndex >= group->nodes_size())
		{
			m_PlainGroupIterator++;
			m_GroupNodeIndex = 0;

			updateGroupMode();
		}
		break;
	case NodeType::DenseNode:
		if (m_GroupNodeIndex >= group->dense().id_size())
		{
			m_DenseGroupIterator++;
			m_GroupNodeIndex = 0;

			updateGroupMode();
		}
		break;
	case NodeType::Undefined:
	default:
		m_GroupNodeIndex = 0;
		break;
	}
}

void NodeStreamInputAdaptor::previousNodeIndex()
{
	m_GroupNodeIndex--;

	if (m_GroupNodeIndex >= 0)
		return;

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		if (m_PlainGroupIterator == m_Controller->m_PlainNodesGroups.begin())
		{
			m_GroupMode = NodeType::Undefined;
		}
		else
		{
			m_PlainGroupIterator--;
		}
		break;
	case NodeType::DenseNode:
	case NodeType::Undefined:
	default:
		if (m_DenseGroupIterator == m_Controller->m_DenseNodesGroups.begin())
		{
			m_GroupMode = NodeType::PlainNode;

			if (m_PlainGroupIterator == m_Controller->m_PlainNodesGroups.begin())
			{
				m_GroupMode = NodeType::Undefined;
			}
			else
			{
				m_PlainGroupIterator--;
			}
		}
		else
		{
			m_DenseGroupIterator--;
		}
		break;
	}

	const crosby::binary::PrimitiveGroup * group = getCurrentGroup();

	if (!group)
	{
		m_GroupNodeIndex = 0;
		return;
	}

	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		m_GroupNodeIndex = group->nodes_size() - 1;
		break;
	case NodeType::DenseNode:
		m_GroupNodeIndex = group->dense().id_size() - 1;
		break;
	case NodeType::Undefined:
	default:
		m_GroupNodeIndex = 0;
		break;
	}

}

crosby::binary::PrimitiveGroup * NodeStreamInputAdaptor::getCurrentGroup() const
{
	switch (m_GroupMode)
	{
	case NodeType::PlainNode:
		return (m_PlainGroupIterator == m_Controller->m_PlainNodesGroups.end()) ?
			nullptr : *m_PlainGroupIterator;
	case NodeType::DenseNode:
		return (m_DenseGroupIterator == m_Controller->m_DenseNodesGroups.end()) ?
			nullptr : m_DenseGroupIterator->group();
	case NodeType::Undefined:
	default:
		return nullptr;
	}
}

} // namespace osmpbf
