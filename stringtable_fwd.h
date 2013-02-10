#ifndef OSMPBF_STRINGTABLE_FDW_H
#define OSMPBF_STRINGTABLE_FDW_H

#include <generics/store_fwd.h>

namespace osmpbf {
	typedef generics::Store< std::string > StringTable;
}
#endif
