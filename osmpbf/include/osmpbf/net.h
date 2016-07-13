#ifndef OSMPBF_NET_H
#define OSMPBF_NET_H
#include <cstdint>

//implementations of functions declared in this file are OS specificc

namespace osmpbf {

uint32_t ntohl(uint32_t v);
uint32_t htonl(uint32_t v);

}//end namespace osmpbf

#endif