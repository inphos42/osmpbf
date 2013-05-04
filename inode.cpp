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

#include "inode.h"

#include "primitiveblockinputadaptor.h"
#include "osmformat.pb.h"

namespace osmpbf {

// INode

	INode::INode() : IPrimitive() {}
	INode::INode(const INode & other) : IPrimitive(other) {}
	INode::INode(AbstractNodeInputAdaptor * data) : IPrimitive(data) {}

// INodeStream

	INodeStream::INodeStream() : INode() {}
	INodeStream::INodeStream(const INodeStream & other) : INode(other) {}
	INodeStream::INodeStream(PrimitiveBlockInputAdaptor * controller) : INode(new NodeStreamInputAdaptor(controller)) {}

// NodeStreamInputAdaptor

	NodeStreamInputAdaptor::NodeStreamInputAdaptor(PrimitiveBlockInputAdaptor * controller) :
		AbstractNodeInputAdaptor(controller),
		m_PlainNodes(controller->m_PlainNodesGroup),
		m_DenseNodes(controller->m_DenseNodesGroup),
		m_Index(0),
		m_DenseIndex(0),
		m_PlainNodesSize(m_PlainNodes ? m_PlainNodes->nodes_size() : 0),
		m_DenseNodesSize(m_DenseNodes ? m_DenseNodes->dense().id_size() : 0),
		m_Id(0),
		m_Lat(0), m_Lon(0),
		m_WGS84Lat(0), m_WGS84Lon(0)
	{
		if (isNull())
			return;

		m_DenseIndex = m_Index - m_PlainNodesSize;

		if (m_PlainNodes) {
			m_Id = m_PlainNodes->nodes(0).id();
			m_Lat = m_PlainNodes->nodes(0).lat();
			m_Lon = m_PlainNodes->nodes(0).lon();
		}
		else if (m_DenseNodes) {
			m_Id = m_DenseNodes->dense().id(0);
			m_Lat = m_DenseNodes->dense().lat(0);
			m_Lon = m_DenseNodes->dense().lon(0);
		}

		m_WGS84Lat = m_Controller->toWGS84Lati(m_Lat);
		m_WGS84Lon = m_Controller->toWGS84Loni(m_Lon);
	}

	void NodeStreamInputAdaptor::next() {
		m_Index++;

		if (isNull())
			return;

		m_DenseIndex = m_Index - m_PlainNodesSize;
		if (m_DenseIndex < 0) {
			m_Id = m_PlainNodes->nodes(m_Index).id();
			m_Lat = m_PlainNodes->nodes(m_Index).lat();
			m_Lon = m_PlainNodes->nodes(m_Index).lon();
		}
		else if (m_DenseIndex > 0) {
			if (m_Controller->denseNodesUnpacked()) {
				m_Id = m_DenseNodes->dense().id(m_DenseIndex);
				m_Lat = m_DenseNodes->dense().lat(m_DenseIndex);
				m_Lon = m_DenseNodes->dense().lon(m_DenseIndex);
			}
			else {
				m_Id += m_DenseNodes->dense().id(m_DenseIndex);
				m_Lat += m_DenseNodes->dense().lat(m_DenseIndex);
				m_Lon += m_DenseNodes->dense().lon(m_DenseIndex);
			}
		}
		else {
			m_Id = m_DenseNodes->dense().id(0);
			m_Lat = m_DenseNodes->dense().lat(0);
			m_Lon = m_DenseNodes->dense().lon(0);
		}

		m_WGS84Lat = m_Controller->toWGS84Lati(m_Lat);
		m_WGS84Lon = m_Controller->toWGS84Loni(m_Lon);
	}

	void NodeStreamInputAdaptor::previous() {
		m_Index--;

		if (isNull())
			return;

		m_DenseIndex = m_Index - m_PlainNodesSize;
		if (m_DenseIndex < 0) {
			m_Id = m_PlainNodes->nodes(m_Index).id();
			m_Lat = m_PlainNodes->nodes(m_Index).lat();
			m_Lon = m_PlainNodes->nodes(m_Index).lon();
		}
		else if (m_DenseIndex > 0) {
			if (m_Controller->denseNodesUnpacked()) {
				m_Id = m_DenseNodes->dense().id(m_DenseIndex);
				m_Lat = m_DenseNodes->dense().lat(m_DenseIndex);
				m_Lon = m_DenseNodes->dense().lon(m_DenseIndex);
			}
			else {
				m_Id -= m_DenseNodes->dense().id(m_DenseIndex + 1);
				m_Lat -= m_DenseNodes->dense().lat(m_DenseIndex + 1);
				m_Lon -= m_DenseNodes->dense().lon(m_DenseIndex + 1);
			}
		}
		else {
			m_Id = m_DenseNodes->dense().id(0);
			m_Lat = m_DenseNodes->dense().lat(0);
			m_Lon = m_DenseNodes->dense().lon(0);
		}

		m_WGS84Lat = m_Controller->toWGS84Lati(m_Lat);
		m_WGS84Lon = m_Controller->toWGS84Loni(m_Lon);
	}

	int NodeStreamInputAdaptor::tagsSize() const {
		if (m_DenseIndex < 0)
			return m_PlainNodes->nodes(m_Index).keys_size();
		else
			return m_DenseNodes->dense().keys_vals_size() ? m_Controller->queryDenseNodeKeyValIndex(m_DenseIndex * 2 + 1) : 0;
	}

	uint32_t NodeStreamInputAdaptor::keyId(int index) const {
		if (index < 0 || index > tagsSize())
			return 0;

		if (m_DenseIndex < 0)
			return m_PlainNodes->nodes(m_Index).keys(index);
		else
			return m_DenseNodes->dense().keys_vals_size() ? m_DenseNodes->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_DenseIndex * 2) + index * 2) : 0;
	}

	uint32_t NodeStreamInputAdaptor::valueId(int index) const {
		if (index < 0 || index > tagsSize())
			return 0;

		if (m_DenseIndex < 0)
			return m_PlainNodes->nodes(m_Index).vals(index);
		else
			return m_DenseNodes->dense().keys_vals_size() ? m_DenseNodes->dense().keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_DenseIndex * 2) + index * 2 + 1) : 0;
	}

// PlainNodeInputAdaptor

	PlainNodeInputAdaptor::PlainNodeInputAdaptor() : AbstractNodeInputAdaptor() {}
	PlainNodeInputAdaptor::PlainNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::Node & data) :
		AbstractNodeInputAdaptor(controller), m_Data(&data) {}

	int64_t PlainNodeInputAdaptor::id() {
		return m_Data->id();
	}

	int64_t PlainNodeInputAdaptor::lati() {
		return m_Controller->toWGS84Lati(m_Data->lat());
	}

	int64_t PlainNodeInputAdaptor::loni() {
		return m_Controller->toWGS84Loni(m_Data->lon());
	}

	double PlainNodeInputAdaptor::latd() {
		return m_Controller->toWGS84Latd(m_Data->lat());
	}

	double PlainNodeInputAdaptor::lond() {
		return m_Controller->toWGS84Lond(m_Data->lon());
	}

	int64_t PlainNodeInputAdaptor::rawLat() const {
		return m_Data->lat();
	}

	int64_t PlainNodeInputAdaptor::rawLon() const {
		return m_Data->lon();
	}

	int PlainNodeInputAdaptor::tagsSize() const {
		return m_Data->keys_size();
	}

	uint32_t PlainNodeInputAdaptor::keyId(int index) const {
		return m_Data->keys(index);
	}

	uint32_t PlainNodeInputAdaptor::valueId(int index) const {
		return m_Data->vals(index);
	}

// DenseNodeInputAdaptor

	DenseNodeInputAdaptor::DenseNodeInputAdaptor() : AbstractNodeInputAdaptor(), m_HasCachedId(false), m_HasCachedLat(false), m_HasCachedLon(false) {}
	DenseNodeInputAdaptor::DenseNodeInputAdaptor(PrimitiveBlockInputAdaptor * controller, const crosby::binary::DenseNodes & data, int position) :
		AbstractNodeInputAdaptor(controller), m_Data(&data), m_Index(position), m_HasCachedId(false), m_HasCachedLat(false), m_HasCachedLon(false) {}

	bool DenseNodeInputAdaptor::isNull() const {
		return AbstractPrimitiveInputAdaptor::isNull() || !m_Data || (m_Index < 0) || (m_Index > m_Data->id_size());
	}

	int64_t DenseNodeInputAdaptor::id() {
		if (m_Controller->denseNodesUnpacked())
			return m_Data->id(m_Data->id(m_Index));

		if (!m_HasCachedId) {
			m_CachedId = m_Data->id(0);
			for (int i = 1; i < m_Index+1; i++)
				m_CachedId += m_Data->id(i);
			m_HasCachedId = true;
		}

		return m_CachedId;
	}

	int64_t DenseNodeInputAdaptor::lati() {
		if (m_Controller->denseNodesUnpacked())
			return m_Controller->toWGS84Lati(m_Data->lat(m_Index));

		if (!m_HasCachedLat) {
			m_CachedLat = m_Data->lat(0);
			for (int i = 1; i < m_Index+1; i++)
				m_CachedLat += m_Data->lat(i);
			m_HasCachedLat = true;
		}

		return m_Controller->toWGS84Lati(m_CachedLat);
	}

	int64_t DenseNodeInputAdaptor::loni() {
		if (m_Controller->denseNodesUnpacked())
			return m_Controller->toWGS84Loni(m_Data->lon(m_Index));

		if (!m_HasCachedLon) {
			m_CachedLon = m_Data->lon(0);
			for (int i = 1; i < m_Index+1; i++)
				m_CachedLon += m_Data->lon(i);
			m_HasCachedLon = true;
		}

		return m_Controller->toWGS84Loni(m_CachedLon);
	}

	double DenseNodeInputAdaptor::latd() {
		return lati() * .000000001;
	}

	double DenseNodeInputAdaptor::lond() {
		return loni() * .000000001;
	}

	int64_t DenseNodeInputAdaptor::rawLat() const {
		return m_Data->lat(m_Index);
	}

	int64_t DenseNodeInputAdaptor::rawLon() const {
		return m_Data->lon(m_Index);
	}

	int DenseNodeInputAdaptor::tagsSize() const {
		return (!m_Data->keys_vals_size()) ? 0 : m_Controller->queryDenseNodeKeyValIndex(m_Index * 2 + 1);
	}

	uint32_t DenseNodeInputAdaptor::keyId(int index) const {
		return (index < 0 || index >= tagsSize()) ? 0 : m_Data->keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_Index * 2) + index * 2);
	}

	uint32_t DenseNodeInputAdaptor::valueId(int index) const {
		return (index < 0 || index >= tagsSize()) ? 0 : m_Data->keys_vals(m_Controller->queryDenseNodeKeyValIndex(m_Index * 2) + index * 2 + 1);
	}
}
