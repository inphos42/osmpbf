#ifndef OSMPBF_TYPELIMITS_H
#define OSMPBF_TYPELIMITS_H

#include <cstdint>

namespace osmpbf {

#ifdef __LP64__
typedef uint64_t OffsetType;
typedef int64_t NegativeOffsetType;
#define MAX_SIZE_FOR_FULL_MMAP 0xFFFFFFFF
#define CHUNKED_MMAP_EXPONENT 23
#else
typedef uint32_t OffsetType;
typedef int32_t NegativeOffsetType;
#define MAX_SIZE_FOR_FULL_MMAP 0x3FFFFFFF
#define CHUNKED_MMAP_EXPONENT 23
#endif

}//end namespace


#endif