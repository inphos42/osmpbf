#ifndef OSMPBF_STRINGTABLE_H
#define OSMPBF_STRINGTABLE_H

#include <map>
#include <deque>
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

		uint32_t insert(const std::string & value);

		void remove(uint32_t id);
		void remove(const std::string & value);
		void remove(StringTableEntry * entry);

		bool contains(const std::string & value) const;

		void clear();

		uint32_t id(const std::string & value) const;

		inline std::map<uint32_t, StringTableEntry *>::const_iterator begin() const { return m_Entries.cbegin(); }
		inline std::map<uint32_t, StringTableEntry *>::const_iterator end() const { return m_Entries.cend(); }

		inline const std::string & query(uint32_t id) const { return m_Entries.at(id)->value; }
		inline const std::string & operator[](uint32_t id) const { return query(id); }

		inline std::size_t size() const { return m_Entries.size(); }

		inline uint32_t maxId() const { return m_IdCounter; }
	protected:
		std::map<uint32_t, StringTableEntry *> m_Entries;
		std::map<std::string, uint32_t> m_IdMap;
		std::deque<uint32_t> m_FreeIds;

		uint32_t m_IdCounter;
	private:
		StringTable(const StringTable & other);
		StringTable & operator=(const StringTable & other);
	};
}

#endif
