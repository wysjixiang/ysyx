#ifndef __FS_H__
#define __FS_H__

#include <common.h>



#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

#endif

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

// typedef creates a name for ReadFn() function pointer and the name is ReadFn
typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);


typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t disk_ptr;
} Finfo;