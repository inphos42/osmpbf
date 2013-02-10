#ifndef OSMPBF_STRINGTABLE_FDW_H
#define OSMPBF_STRINGTABLE_FDW_H
namespace generics {
	template< typename > class Store;
}

namespace osmpbf {
	typedef generics::Store< std::string > StringTable;
}
#endif
