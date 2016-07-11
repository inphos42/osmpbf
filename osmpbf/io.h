#ifndef OSMPBF_IO_H
#define OSMPBF_IO_H
#include <osmpbf/typelimits.h>

namespace osmpbf {

typedef enum { IO_OPEN_READ_ONLY=00, IO_OPEN_WRITE_ONLY=01, IO_OPEN_CREATE=0100, IO_OPEN_TRUNCATE=01000} IoOpenFlags;
typedef enum { IO_SEEK_SET=0x0, IO_SEEK_CUR=0x1 } IoSeekOptions;
typedef enum { MM_PROT_READ=0x1 } MmapProtections;
typedef enum { MM_MAP_SHARED=0x1 } MmapSharing;

///@param oflag combination of IoOpenFlags
int open(const char * path, int oflag);

///@param oflag combination of IoOpenFlags
int open(const char * path, int oflag, int omode);

int close(int fd);


int lseek(int fd, OffsetType offset, int whence);

SignedSizeType write(int fd, const void * buffer, SizeType count);



///@param protection expects a combination of MmapProtections
///@param flags expects a combination of MmapSharing
void * mmap (void * addr, SizeType len, int protection, int flags, int fd, OffsetType offset);

int munmap(void * addr, SizeType len);


}//end namespace osmpbf

#endif