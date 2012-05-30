#include "primitiveblockoutputadaptor.h"
#include "osmformat.pb.h"
#include "iway.h"
#include "stringtable.h"

namespace osmpbf {

// PrimitiveBlockOutputAdaptor

	PrimitiveBlockOutputAdaptor::PrimitiveBlockOutputAdaptor() : m_WaysGroup(NULL) {
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

	OWay PrimitiveBlockOutputAdaptor::createWay() {
		if (!m_WaysGroup)
			m_WaysGroup = m_PrimitiveBlock->mutable_primitivegroup()->Add();

		return OWay(new WayOutputAdaptor(this, m_WaysGroup->mutable_ways()->Add()));
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

	int PrimitiveBlockOutputAdaptor::waysSize() {
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

	bool PrimitiveBlockOutputAdaptor::flush(std::string & output) {
		if (!m_PrimitiveBlock->IsInitialized())
			return false;

		// prepare string table output cache
		uint32_t * stringIdTable = new uint32_t[m_StringTable->maxId()];
		{
			for (uint32_t i = 0; i < m_StringTable->maxId(); i++)
				stringIdTable[i] = 0;

			// build string id table and fill string table output cache
			int newId = 1;
			std::map<int, StringTableEntry *>::const_iterator stringIt = m_StringTable->begin();
			while (stringIt != m_StringTable->end()) {
				stringIdTable[stringIt->first] = newId;
				m_PrimitiveBlock->mutable_stringtable()->add_s(stringIt->second->value);
				newId++;
				++stringIt;
			}

			// we don't need the old string table anymore
			delete m_StringTable;
			m_StringTable = new StringTable();
		}

		// TODO prepare nodes

		// prepare ways
		{
			google::protobuf::RepeatedPtrField<Way>::iterator wayIt = m_WaysGroup->mutable_ways()->begin();
			int realSize = 0;
			while (wayIt != m_WaysGroup->mutable_ways()->end()) {
				// encode and clean refs
				realSize = deltaEncodeClean<int64_t>(wayIt->mutable_refs()->mutable_data(), wayIt->mutable_refs()->mutable_data() + wayIt->refs_size(), -1);
				wayIt->mutable_refs()->Truncate(realSize);

				// clean keys
				realSize = cleanUp<uint32_t>(wayIt->mutable_keys()->mutable_data(), wayIt->mutable_keys()->mutable_data() + wayIt->keys_size(), 0);
				wayIt->mutable_keys()->Truncate(realSize);

				// clean vals
				realSize = cleanUp<uint32_t>(wayIt->mutable_vals()->mutable_data(), wayIt->mutable_vals()->mutable_data() + wayIt->vals_size(), 0);
				wayIt->mutable_vals()->Truncate(realSize);

				// correct string ids
				for (int i = 0; i < wayIt->keys_size(); i++) {
					wayIt->set_keys(i, stringIdTable[wayIt->keys(i)]);
					wayIt->set_vals(i, stringIdTable[wayIt->vals(i)]);
				}

				++wayIt;
			}
		}

		output = m_PrimitiveBlock->SerializeAsString();

		delete m_PrimitiveBlock;
		m_PrimitiveBlock = new PrimitiveBlock();
		return true;
	}

}
