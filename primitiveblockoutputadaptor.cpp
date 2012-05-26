#include "primitiveblockoutputadaptor.h"
#include "osmformat.pb.h"
#include "iway.h"
#include "stringtable.h"

namespace osmpbf {

// PrimitiveBlockOutputAdaptor

	PrimitiveBlockOutputAdaptor::PrimitiveBlockOutputAdaptor() : m_WaysGroup(NULL) {
		GOOGLE_PROTOBUF_VERIFY_VERSION;

		m_PrimitiveBlock = new PrimitiveBlock();
		m_StringTable = new StringTable();
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
		for (int i = 0; templateIWay.tagsSize(); i++)
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
		Element * target = it;
		Element prev = 0;
		Element delta;

		while (it != to) {
			if (*it != clearValue) {
				delta = *it - prev;
				prev = *it;
				*target = delta;

				target++;
			}

			it++;
		}

		return target - from + 1;
	}

	bool PrimitiveBlockOutputAdaptor::flush(std::string & output) {
		if (!m_PrimitiveBlock->IsInitialized())
			return false;

		// prepare string table
		int * stringIdTable = new int[m_StringTable->maxId()];
		{
			int newId = 1;
			std::map<int, StringTableEntry *>::const_iterator stringIt = m_StringTable->begin();
			while (stringIt != m_StringTable->end()) {
				stringIdTable[stringIt->first] = newId;
				newId++;
				++stringIt;
			}
		}

		// TODO prepare nodes

		// prepare ways
		{
			google::protobuf::RepeatedPtrField<Way>::iterator wayIt = m_WaysGroup->mutable_ways()->begin();
			int realRefsSize = 0;
			while (wayIt != m_WaysGroup->mutable_ways()->end()) {
				// encode refs
				realRefsSize = deltaEncodeClean<int64_t>(wayIt->mutable_refs()->mutable_data(), wayIt->mutable_refs()->mutable_data() + wayIt->refs_size(), 0);
				wayIt->mutable_refs()->Truncate(realRefsSize);

				++wayIt;
			}
		}

		output = m_PrimitiveBlock->SerializeAsString();
		return output.length();
	}

}
