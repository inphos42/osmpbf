#include "stringtable.h"

namespace osmpbf {
	StringTable::StringTable() : m_IdCounter(1) {}
	StringTable::~StringTable() {
		std::map<int, StringTableEntry *>::const_iterator stringIt = m_Entries.cbegin();
		while (stringIt != m_Entries.cend()) {
			delete stringIt->second;
			++stringIt;
		}
	}

	int StringTable::insert(const std::string & value) {
		std::map<std::string, int>::const_iterator target = m_IdMap.find(value);
		int result;
		StringTableEntry * entry;

		if (target == m_IdMap.end()) {
			if (m_FreeIds.empty()) {
				result = m_IdCounter;
				m_IdCounter++;
			}
			else {
				result = m_FreeIds.front();
				m_FreeIds.pop();
			}

			entry = new StringTableEntry(value, 0);

			m_Entries.insert(std::pair<int, StringTableEntry *>(result, entry));
			m_IdMap.insert(std::pair<std::string, int>(value, m_Entries.size()));
		}
		else {
			result = target->second;
			entry = m_Entries[result];
		}

		entry->references++;

		return result;
	}

	void StringTable::remove(uint32_t id) {
		if (!id)
			return;

		std::map<int, StringTableEntry *>::iterator entryIt = m_Entries.find(id);
		StringTableEntry * entry = entryIt->second;

		entry->references--;

		if (entry->references < 1) {
			m_FreeIds.push(id);
			m_IdMap.erase(entry->value);
			m_Entries.erase(entryIt);
			delete entry;
		}
	}

	void StringTable::remove(const std::string & value) {
		std::map<std::string, int>::iterator target = m_IdMap.find(value);
		if (target == m_IdMap.end())
			return;

		std::map<int, StringTableEntry *>::iterator entryIt = m_Entries.find(target->second);
		StringTableEntry * entry = entryIt->second;

		entry->references--;

		if (entry->references < 1) {
			m_FreeIds.push(entryIt->first);
			m_IdMap.erase(target);
			m_Entries.erase(entryIt);
			delete entry;
		}
	}

	void StringTable::remove(StringTableEntry * entry) {
		std::map<std::string, int>::iterator target = m_IdMap.find(entry->value);
		if (target == m_IdMap.end())
			return;

		std::map<int, StringTableEntry *>::iterator entryIt = m_Entries.find(target->second);

		m_FreeIds.push(entryIt->first);
		m_IdMap.erase(target);
		m_Entries.erase(entryIt);
		delete entry;
	}

	bool StringTable::contains(const std::string & value) const {
		return m_IdMap.count(value);
	}
}
