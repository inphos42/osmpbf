#include "primitiveblockoutputadaptor.h"
#include "osmformat.pb.h"
#include "iway.h"
#include "oway.h"
#include "stringtable.h"
#include "onode.h"
#include "inode.h"

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
		m_PrimitiveBlock = new PrimitiveBlock();

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
		PrimitiveGroup * targetGroup;
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

	int PrimitiveBlockOutputAdaptor::nodesSize (NodeType type) const {
		switch (type) {
		case PlainNode:
			return m_PlainNodesGroup ? m_PlainNodesGroup->nodes_size() : 0;
		case DenseNode:
			return m_DenseNodesGroup ? m_DenseNodesGroup->nodes_size() : 0;
		default:
			return -1;
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
		std::map<uint32_t, StringTableEntry *>::const_iterator stringIt = m_StringTable->begin();
		while (stringIt != m_StringTable->end()) {
			stringIdTable[stringIt->first] = newId;
			m_PrimitiveBlock->mutable_stringtable()->add_s(stringIt->second->value);
			newId++;
			++stringIt;
		}

		// we don't need the old string table anymore
		m_StringTable->clear();

		return stringIdTable;
	}

	void PrimitiveBlockOutputAdaptor::prepareNodes(PrimitiveGroup * nodesGroup, uint32_t * stringIdTable) {
		google::protobuf::RepeatedPtrField<Node>::iterator nodeIt = nodesGroup->mutable_nodes()->begin();
		while (nodeIt != nodesGroup->mutable_nodes()->end()) {
			// calculate coordinates
			nodeIt->set_lat((nodeIt->lat() - m_PrimitiveBlock->lat_offset()) / m_PrimitiveBlock->granularity());
			nodeIt->set_lon((nodeIt->lon() - m_PrimitiveBlock->lon_offset()) / m_PrimitiveBlock->granularity());

			cleanUpTags<Node>(*nodeIt, stringIdTable);
			++nodeIt;
		}
	}

	bool PrimitiveBlockOutputAdaptor::flush(std::string & output) {
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
			google::protobuf::RepeatedPtrField<Node>::const_iterator nodeIt = m_DenseNodesGroup->nodes().begin();
			while (nodeIt != m_DenseNodesGroup->nodes().end()) {
				m_DenseNodesGroup->mutable_dense()->add_id(nodeIt->id() - prevId);
				m_DenseNodesGroup->mutable_dense()->add_lat(nodeIt->lat() - prevLat);
				m_DenseNodesGroup->mutable_dense()->add_lon(nodeIt->lon() - prevLon);

				prevId = nodeIt->id();
				prevLat = nodeIt->lat();
				prevLon = nodeIt->lon();

				const uint32_t * keys = nodeIt->keys().data();
				const uint32_t * vals = nodeIt->vals().data();
				for (int i = 0; i < nodeIt->keys_size(); ++i) {
					m_DenseNodesGroup->mutable_dense()->add_keys_vals(keys[i]);
					m_DenseNodesGroup->mutable_dense()->add_keys_vals(vals[i]);
				}
				m_DenseNodesGroup->mutable_dense()->add_keys_vals(0);

				++nodeIt;
			}

			m_DenseNodesGroup->clear_nodes();
		}

		// prepare ways
		if (m_WaysGroup) {
			google::protobuf::RepeatedPtrField<Way>::iterator wayIt = m_WaysGroup->mutable_ways()->begin();
			int realSize;
			while (wayIt != m_WaysGroup->mutable_ways()->end()) {
				// encode and clean refs
				realSize = deltaEncodeClean<int64_t>(wayIt->mutable_refs()->mutable_data(), wayIt->mutable_refs()->mutable_data() + wayIt->refs_size(), -1);
				wayIt->mutable_refs()->Truncate(realSize);

				cleanUpTags<Way>(*wayIt, stringIdTable);

				++wayIt;
			}
		}

		delete[] stringIdTable;

		output = m_PrimitiveBlock->SerializeAsString();

		delete m_PrimitiveBlock;
		m_PrimitiveBlock = new PrimitiveBlock();

		m_PlainNodesGroup = NULL;
		m_DenseNodesGroup = NULL;
		m_WaysGroup = NULL;
		m_RelationsGroup = NULL;

		return true;
	}

	PrimitiveBlockOutputAdaptor & PrimitiveBlockOutputAdaptor::operator<<(INode & node) {
		 createNode(node); return *this;
	}

	PrimitiveBlockOutputAdaptor & PrimitiveBlockOutputAdaptor::operator<<(IWay & way) {
		 createWay(way); return *this;
	}
}
