#include "io.h"
#ifdef _WIN32
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#endif

namespace osmpbf {
namespace lx {
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

int open(const char * path, int oflag) {
	return ::open(path, __open_flags_from_my_open_flags(oflag));
}

int open(const char * path, int oflag, int omode) {
	return ::open(path, __open_flags_from_my_open_flags(oflag), omode);
}

int close(int fd) {
	return ::close(fd);
}

int lseek(int fd, OffsetType offset, int whence) {
	int wh = 0;
	if (whence & IO_SEEK_CUR) {
		wh |= SEEK_CUR;
	}
	if (whence & IO_SEEK_SET) {
		wh |= SEEK_SET;
	}
	return ::lseek(fd, offset, wh);
}

size_t write(int fd, const void * buffer, SizeType count) {
	return write(fd, buffer, count);
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

}//end namespace lx

namespace windows {

} //end namespace windows

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

int munmap(void * addr, SizeType len) {
	return MY_NAME_SPACE::munmap(addr, len);
}

#undef MY_NAME_SPACE

}//end namespace