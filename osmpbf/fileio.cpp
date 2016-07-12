#include <osmpbf/fileio.h>
#include <fcntl.h>

#ifdef _WIN32
	#include <WinSock2.h>
	#include <mman.h>
	#include <fstream> 
	#include <cstddef>
	#include <stdint.h>
	#include <io.h>
#endif

#ifndef _WIN32
	#include <unistd.h>
	#include <sys/mman.h>
#endif

namespace osmpbf {

namespace common {

inline int __open_flags_from_my_open_flags(int oflag) {
	int r = 0;
	if (oflag & IO_OPEN_WRITE_ONLY) {
		r |= O_WRONLY;
	}
	if (oflag & IO_OPEN_CREATE) {
		r |= O_CREAT;
	}
	if (oflag & IO_OPEN_TRUNCATE) {
		r |= O_TRUNC;
	}
	if (oflag & IO_OPEN_READ_ONLY) {
		r |= O_RDONLY;
	}
	return r;
}

inline int __seek_flags_from_my_seek_flags(int whence) {
	int r = 0;
	if (whence & IO_SEEK_CUR) {
		r |= SEEK_CUR;
	}
	if (whence & IO_SEEK_SET) {
		r |= SEEK_SET;
	}
	return r;
}

void * mmap (void * addr, SizeType len, int protection, int flags, int fd, OffsetType offset) {
	int prot = 0;
	int fl = 0;
	if (protection & MM_PROT_READ) {
		prot |= PROT_READ;
	}
	if (flags & MM_MAP_SHARED) {
		fl |= MAP_SHARED;
	}
	return ::mmap(addr, len, prot, fl, fd, offset);
}

int munmap(void * addr, SizeType len) {
	return ::munmap(addr, len);
}

bool validMmapAddress(void* addr) {
	return addr != MAP_FAILED;
}

} //end namespace common

#ifndef _WIN32

namespace lx {

int open(const char * path, int oflag) {
	return ::open(path, common::__open_flags_from_my_open_flags(oflag));
}

int open(const char * path, int oflag, int omode) {
	return ::open(path, common::__open_flags_from_my_open_flags(oflag), omode);
}

int close(int fd) {
	return ::close(fd);
}

int lseek(int fd, OffsetType offset, int whence) {
	return ::lseek(fd, offset, common::__seek_flags_from_my_seek_flags(whence));
}

SignedSizeType write(int fd, const void * buffer, SizeType count) {
	return ::write(fd, buffer, count);
}

using common::mmap;
using common::munmap;
using common::validMmapAddress;

}//end namespace lx

#endif

#ifdef _WIN32
namespace windows {

int open(const char * path, int oflag) {
	return _open(path, common::__open_flags_from_my_open_flags(oflag));
}

int open(const char * path, int oflag, int omode) {
	return _open(path, common::__open_flags_from_my_open_flags(oflag), omode);
}

int close(int fd) {
	return _close(fd);
}

int lseek(int fd, OffsetType offset, int whence) {
	return _lseek(fd, offset, common::__seek_flags_from_my_seek_flags(whence));
}

SignedSizeType write(int fd, const void * buffer, SizeType count) {
	return _write(fd, buffer, count);
}

using common::mmap;
using common::munmap;
using common::validMmapAddress;

} //end namespace windows
#endif

#ifdef _WIN32
	#define MY_NAME_SPACE windows
#else
	#define MY_NAME_SPACE lx
#endif

int open(const char* path, int oflag) {
	return MY_NAME_SPACE::open(path, oflag);
}

int open(const char* path, int oflag, int omode) {
	return MY_NAME_SPACE::open(path, oflag, omode);
}

int close(int fd) {
	return MY_NAME_SPACE::close(fd);
}

int lseek(int fd, OffsetType offset, int whence) {
	return MY_NAME_SPACE::lseek(fd, offset, whence);
}

SignedSizeType write(int fd, const void * buffer, SizeType count) {
	return MY_NAME_SPACE::write(fd, buffer, count);
}

void * mmap (void* addr, SizeType len, int protection, int flags, int fd, OffsetType offset) {
	return MY_NAME_SPACE::mmap(addr, len, protection, flags, fd, offset);
}

bool validMmapAddress(void* addr) {
	return MY_NAME_SPACE::validMmapAddress(addr);
}

int munmap(void * addr, SizeType len) {
	return MY_NAME_SPACE::munmap(addr, len);
}

#undef MY_NAME_SPACE

}//end namespace