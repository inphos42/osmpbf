#ifndef OSMPBF_NET_H
#define OSMPBF_NET_H
#include <cstdint>

//implementations of functions declared in this file are OS specific

namespace osmpbf {

uint32_t net2hostLong(uint32_t v);

uint32_t host2NetLong(uint32_t v);

}//end namespace osmpbf

#endif