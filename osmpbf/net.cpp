#include <osmpbf/net.h>

#ifdef _WIN32
	#include <WinSock2.h>
#endif

#ifndef _WIN32
	#include <netinet/in.h>
#endif

namespace osmpbf {

uint32_t net2hostLong(uint32_t v) {
	return ::ntohl(v);
}

uint32_t host2NetLong(uint32_t v) {
	return ::htonl(v);
}

}//end namespace osmpbf