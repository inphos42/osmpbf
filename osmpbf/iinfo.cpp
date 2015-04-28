#include <osmpbf/iinfo.h>
#include "osmformat.pb.h"

namespace osmpbf {

IInfo::IInfo(const crosby::binary::Info& info) :
m_version(info.has_version() ? info.version() : -1),
m_timestamp(info.has_timestamp() ? info.timestamp() : -1),
m_changeset(info.has_changeset() ? info.changeset() : -1),
m_userId(info.has_uid() ? info.uid() : -1),
m_userStringId(info.has_user_sid() ? info.user_sid() : -1)
{}


}//end namespace