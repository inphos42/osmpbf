#ifndef OSMPBF_TYPELIMITS_H
#define OSMPBF_TYPELIMITS_H

#include <cstdint>

namespace osmpbf {

#ifdef __LP64__
typedef uint64_t OffsetType;
typedef int64_t NegativeOffsetType;
#else
typedef uint32_t OffsetType;
typedef int32_t NegativeOffsetType;
#endif

}//end namespace


#endif