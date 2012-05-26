#ifndef OSMPBF_STRINGTABLE_H
#define OSMPBF_STRINGTABLE_H

#include <map>
#include <queue>
#include <string>
#include <cstdint>

namespace osmpbf {
	struct StringTableEntry {
		std::string value;
		int references;

		StringTableEntry(const std::string & val, int refs = 0) : value(val), references(refs) {}
	};

	class StringTable {
	public:
		StringTable();
		virtual ~StringTable();

		int insert(const std::string & value);

		void remove(uint32_t id);
		void remove(const std::string & value);
		void remove(StringTableEntry * entry);

		bool contains(const std::string & value) const;

		inline std::map<int, StringTableEntry *>::const_iterator begin() const { return m_Entries.begin(); }
		inline std::map<int, StringTableEntry *>::const_iterator end() const { return m_Entries.end(); }

		inline std::string & operator[](int id) { return m_Entries[id]->value; }

		inline uint32_t maxId() const { return m_IdCounter; }
	protected:
		std::map<int, StringTableEntry *> m_Entries;
		std::map<std::string, int> m_IdMap;
		std::queue<int> m_FreeIds;

		uint32_t m_IdCounter;
	private:
		StringTable(const StringTable & other);
		StringTable & operator=(const StringTable & other);
	};
}

#endif
