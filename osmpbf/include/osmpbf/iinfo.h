#ifndef OSMPBF_IINFO_H
#define OSMPBF_IINFO_H
#include <stdint.h>
#include <osmpbf/pbf_prototypes.h>

namespace osmpbf {

class IInfo {
public:
	IInfo() : m_version(-1), m_timestamp(-1), m_changeset(-1), m_userId(-1), m_userStringId(-1) {}
	IInfo(const ::crosby::binary::Info & info);
	inline int32_t version() const { return m_version; }
	inline int32_t timestamp() const { return m_timestamp; }
	inline int64_t changeset() const { return m_changeset; }
	inline int32_t userId() const { return m_userId; }
	inline int32_t userStringId() const { return m_userStringId; }
private:
	int32_t m_version;
	int32_t m_timestamp;
	int64_t m_changeset;
	int32_t m_userId;
	int32_t m_userStringId;
};

}//end namespace

#endif