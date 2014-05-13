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

#include <osmpbf/primitiveblockoutputadaptor.h>
#include <osmpbf/iway.h>
#include <osmpbf/oway.h>
#include <osmpbf/onode.h>
#include <osmpbf/inode.h>
#include <generics/store.h>

#include "osmformat.pb.h"


namespace osmpbf {

// PrimitiveBlockOutputAdaptor

	PrimitiveBlockOutputAdaptor::PrimitiveBlockOutputAdaptor() :
		m_PlainNodesGroup(NULL),
		m_DenseNodesGroup(NULL),
		m_WaysGroup(NULL),
		m_RelationsGroup(NULL)
	{
		GOOGLE_PROTOBUF_VERIFY_VERSION;

		m_StringTable = new StringTable();
		m_PrimitiveBlock = new crosby::binary::PrimitiveBlock();

		// add empty string table cache entry
		m_PrimitiveBlock->mutable_stringtable()->add_s(std::string());
	}

	PrimitiveBlockOutputAdaptor::~PrimitiveBlockOutputAdaptor() {
		delete m_StringTable;
		delete m_PrimitiveBlock;
	}

	ONode PrimitiveBlockOutputAdaptor::createNode(NodeType type) {
		switch (type) {
		case DenseNode:
			if (!m_DenseNodesGroup)
				m_DenseNodesGroup = m_PrimitiveBlock->add_primitivegroup();

			return ONode(new NodeOutputAdaptor(this, m_DenseNodesGroup->add_nodes()));
		case PlainNode:
			if (!m_PlainNodesGroup)
				m_PlainNodesGroup = m_PrimitiveBlock->add_primitivegroup();

			return ONode(new NodeOutputAdaptor(this, m_PlainNodesGroup->add_nodes()));
		default:
			return ONode();
		}
	}

	ONode PrimitiveBlockOutputAdaptor::createNode(INode & templateINode) {
		return createNode(templateINode, templateINode.internalNodeType());
	}

	ONode PrimitiveBlockOutputAdaptor::createNode(INode & templateINode, NodeType type) {
		crosby::binary::PrimitiveGroup * targetGroup;
		switch (type) {
		case DenseNode:
			if (!m_DenseNodesGroup)
				m_DenseNodesGroup = m_PrimitiveBlock->add_primitivegroup();
			targetGroup = m_DenseNodesGroup;
			break;
		case PlainNode:
			if (!m_PlainNodesGroup)
				m_PlainNodesGroup = m_PrimitiveBlock->add_primitivegroup();
			targetGroup = m_PlainNodesGroup;
			break;
		default:
			return ONode();
		}

		ONode result = ONode(new NodeOutputAdaptor(this, targetGroup->add_nodes()));

		//set field for new node
		result.setId(templateINode.id());
		result.setLati(templateINode.lati());
		result.setLoni(templateINode.loni());
		for (int i = 0; i < templateINode.tagsSize(); i++)
			result.addTag(templateINode.key(i), templateINode.value(i));

		return result;
	}

	int PrimitiveBlockOutputAdaptor::nodesSize(NodeType type) const {
		switch (type) {
		case PlainNode:
			return m_PlainNodesGroup ? m_PlainNodesGroup->nodes_size() : 0;
		case DenseNode:
			return m_DenseNodesGroup ? m_DenseNodesGroup->nodes_size() : 0;
		default:
			return 0;
		}
	}

	OWay PrimitiveBlockOutputAdaptor::createWay() {
		if (!m_WaysGroup)
			m_WaysGroup = m_PrimitiveBlock->add_primitivegroup();

		return OWay(new WayOutputAdaptor(this, m_WaysGroup->add_ways()));
	}

	OWay PrimitiveBlockOutputAdaptor::createWay(const IWay & templateIWay) {
		OWay result = createWay();

		// set fields for new way
		result.setId(templateIWay.id());
		result.setRefs(templateIWay.refBegin(), templateIWay.refEnd());
		for (int i = 0; i < templateIWay.tagsSize(); i++)
			result.addTag(templateIWay.key(i), templateIWay.value(i));

		return result;
	}

	int PrimitiveBlockOutputAdaptor::waysSize() const {
		return m_WaysGroup ? m_WaysGroup->ways_size() : 0;
	}

	void PrimitiveBlockOutputAdaptor::setGranularity(int32_t value) {
		m_PrimitiveBlock->set_granularity(value);
	}

	void PrimitiveBlockOutputAdaptor::setLatOffset(int64_t value) {
		m_PrimitiveBlock->set_lat_offset(value);
	}

	void PrimitiveBlockOutputAdaptor::setLonOffset(int64_t value) {
		m_PrimitiveBlock->set_lon_offset(value);
	}

	template<typename Element>
	int deltaEncodeClean(Element * from, Element * to, Element clearValue) {
		if (from == to)
			return 0;

		Element * it = from;
		Element * target = it - 1;
		Element prev = 0;
		Element delta;

		while (it != to) {
			if (*it != clearValue) {
				target++;

				delta = *it - prev;
				prev = *it;
				*target = delta;
			}

			it++;
		}

		return target - from + 1;
	}

	template<typename Element>
	void deltaEncode(Element * from, Element * to) {
		if (from == to)
			return;

		Element * it = from;
		Element prev = 0;
		Element delta;

		while (it != to) {
			delta = *it - prev;
			prev = *it;
			*it = delta;

			it++;
		}
	}

	template<typename Element>
	int cleanUp(Element * from, Element * to, Element clearValue) {
		if (from == to)
			return 0;

		Element * it = from;
		Element * target = it - 1;

		while (it != to) {
			if (*it != clearValue) {
				target++;

				if (target != it)
					*target = *it;
			}

			it++;
		}

		return target - from + 1;
	}

	template<typename PrimitiveType>
	void cleanUpTags(PrimitiveType & primitive, uint32_t * stringIdTable) {
		int realSize;

		// clean keys
		realSize = cleanUp<uint32_t>(primitive.mutable_keys()->mutable_data(), primitive.mutable_keys()->mutable_data() + primitive.keys_size(), 0);
		primitive.mutable_keys()->Truncate(realSize);

		// clean vals
		realSize = cleanUp<uint32_t>(primitive.mutable_vals()->mutable_data(), primitive.mutable_vals()->mutable_data() + primitive.vals_size(), 0);
		primitive.mutable_vals()->Truncate(realSize);

		// correct string ids
		for (int i = 0; i < primitive.keys_size(); i++) {
			primitive.set_keys(i, stringIdTable[primitive.keys(i)]);
			primitive.set_vals(i, stringIdTable[primitive.vals(i)]);
		}
	}

	uint32_t * PrimitiveBlockOutputAdaptor::prepareStringTable() {
		uint32_t * stringIdTable = new uint32_t[m_StringTable->maxId()];
		for (uint32_t i = 0; i < m_StringTable->maxId(); i++)
			stringIdTable[i] = 0;

		// build string id table and fill string table output cache
		int newId = 1;
		StringTable::const_iterator stringIt = m_StringTable->cbegin();
		while (stringIt != m_StringTable->cend()) {
			stringIdTable[stringIt->first] = newId;
			m_PrimitiveBlock->mutable_stringtable()->add_s(stringIt->second->value);
			newId++;
			++stringIt;
		}

		// we don't need the old string table anymore
		m_StringTable->clear();

		return stringIdTable;
	}

	void PrimitiveBlockOutputAdaptor::prepareNodes(crosby::binary::PrimitiveGroup * nodesGroup, uint32_t * stringIdTable) {
		int32_t granularity = m_PrimitiveBlock->has_granularity() ? m_PrimitiveBlock->granularity() : 1;
		int64_t latOffset = m_PrimitiveBlock->has_lat_offset() ? m_PrimitiveBlock->lat_offset() : 0;
		int64_t lonOffset = m_PrimitiveBlock->has_lon_offset() ? m_PrimitiveBlock->lon_offset() : 1;

		google::protobuf::RepeatedPtrField<crosby::binary::Node>::iterator nodeIt = nodesGroup->mutable_nodes()->begin();
		while (nodeIt != nodesGroup->mutable_nodes()->end()) {
			// calculate coordinates
			nodeIt->set_lat((nodeIt->lat() - latOffset) / granularity);
			nodeIt->set_lon((nodeIt->lon() - lonOffset) / granularity);

			cleanUpTags<crosby::binary::Node>(*nodeIt, stringIdTable);
			++nodeIt;
		}
	}

	bool PrimitiveBlockOutputAdaptor::flush(std::string & buffer) {
		if (!m_PrimitiveBlock->IsInitialized())
			return false;

		// prepare string table output cache
		uint32_t * stringIdTable = prepareStringTable();

		// prepare plain nodes
		if (m_PlainNodesGroup)
			prepareNodes(m_PlainNodesGroup, stringIdTable);

		// prepare dense nodes
		if (m_DenseNodesGroup) {
			prepareNodes(m_DenseNodesGroup, stringIdTable);

			int64_t prevLat = 0, prevLon = 0, prevId = 0;
			google::protobuf::RepeatedPtrField<crosby::binary::Node>::const_iterator nodeIt = m_DenseNodesGroup->nodes().begin();
			while (nodeIt != m_DenseNodesGroup->nodes().end()) {
				m_DenseNodesGroup->mutable_dense()->add_id(nodeIt->id() - prevId);
				m_DenseNodesGroup->mutable_dense()->add_lat(nodeIt->lat() - prevLat);
				m_DenseNodesGroup->mutable_dense()->add_lon(nodeIt->lon() - prevLon);

				prevId = nodeIt->id();
				prevLat = nodeIt->lat();
				prevLon = nodeIt->lon();

				for (int i = 0; i < nodeIt->keys_size(); ++i) {
					m_DenseNodesGroup->mutable_dense()->add_keys_vals(nodeIt->keys(i));
					m_DenseNodesGroup->mutable_dense()->add_keys_vals(nodeIt->vals(i));
				}
				m_DenseNodesGroup->mutable_dense()->add_keys_vals(0);

				++nodeIt;
			}

			m_DenseNodesGroup->clear_nodes();
		}

		// prepare ways
		if (m_WaysGroup) {
			google::protobuf::RepeatedPtrField<crosby::binary::Way>::iterator wayIt = m_WaysGroup->mutable_ways()->begin();
			int realSize;
			while (wayIt != m_WaysGroup->mutable_ways()->end()) {
				// encode and clean refs
				realSize = deltaEncodeClean<int64_t>(wayIt->mutable_refs()->mutable_data(), wayIt->mutable_refs()->mutable_data() + wayIt->refs_size(), -1);
				wayIt->mutable_refs()->Truncate(realSize);

				cleanUpTags<crosby::binary::Way>(*wayIt, stringIdTable);

				++wayIt;
			}
		}

		delete[] stringIdTable;

		assert(m_PrimitiveBlock->IsInitialized());

		m_PrimitiveBlock->SerializeToString(&buffer);

		delete m_PrimitiveBlock;
		m_PrimitiveBlock = new crosby::binary::PrimitiveBlock();

		m_PlainNodesGroup = NULL;
		m_DenseNodesGroup = NULL;
		m_WaysGroup = NULL;
		m_RelationsGroup = NULL;

		// add empty string table cache entry
		m_PrimitiveBlock->mutable_stringtable()->add_s(std::string());

		return true;
	}

	PrimitiveBlockOutputAdaptor & PrimitiveBlockOutputAdaptor::operator<<(INode & node) {
		 createNode(node); return *this;
	}

	PrimitiveBlockOutputAdaptor & PrimitiveBlockOutputAdaptor::operator<<(IWay & way) {
		 createWay(way); return *this;
	}
}
