#include <common.h>
#include "syscall.h"
#include <fs.h>

// import
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(void const *buf, size_t offset, size_t len);
int get_table_num();


static Finfo *p = NULL;

void get_table_head(Finfo *table){
  p = table;
}

//export
uintptr_t sys_open(uintptr_t name);
uintptr_t sys_read(uintptr_t fd,uintptr_t buf,uintptr_t len);
uintptr_t sys_lseek(uintptr_t fd,uintptr_t offset,uintptr_t whence);
uintptr_t sys_write(uintptr_t fd,uintptr_t buf,uintptr_t len);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR_a0;
  a[1] = c->GPR_a1;
  a[2] = c->GPR_a2;
  a[3] = c->GPR_a7;

  switch (a[3]) {

    case SYS_exit:
      halt(a[0]);
      break;

    case SYS_yield:
      yield();
      c->GPRx = 0; // syscal return 0
      break;

    case SYS_open:
      c->GPR_a0 = sys_open(a[0]);
      break;

    case SYS_lseek:
      c->GPR_a0 = sys_lseek(a[0],a[1],a[2]);
      break;
    
    case SYS_close:
      c->GPR_a0 = 0;
      break;

    case SYS_write:
      // right now. this function is not well behaved!!
      // we just return the size it wants to write, but not consider some cases!!
      // if meet bugs, remember to modify!
      c->GPR_a0 = sys_write(a[0],a[1],a[2]);
      break;

    case SYS_read:
      c->GPR_a0 = sys_read(a[0],a[1],a[2]);
      break;

    case SYS_brk:
      
      // just return 0
      c->GPR_a0 = 0;
      break;

    default: panic("Unhandled syscall ID = %d", a[3]);
  }
}


uintptr_t sys_open(uintptr_t name){

  int num = get_table_num();
  char *_name = (char *)name;
  for(int i=3;i<num;i++){
    if(strcmp(_name,p[i].name) == 0){
      printf("name = %s\n",_name);
      return i;
    }
  }

  printf("File not found!\n");
  assert(0);
  return -1;
}

uintptr_t sys_read(uintptr_t fd,uintptr_t buf,uintptr_t len){
  uintptr_t ret;
  if(fd == FD_STDIN){
    ret = p[fd].read((void *)buf,0,len);
  } else{

    if(len + p[fd].disk_ptr > p[fd].size){
      len = p[fd].size - p[fd].disk_ptr;
    }
    uintptr_t offset = p[fd].disk_offset + p[fd].disk_ptr;
    ret = ramdisk_read((void *)buf, offset, len);
    p[fd].disk_ptr += len;
  }
  return ret;
}

uintptr_t sys_lseek(uintptr_t fd,uintptr_t offset,uintptr_t whence){

  uintptr_t ret = -1;
  int num = get_table_num();
  if(fd >= num){
    printf("fd wrong!\n");
    assert(0);
  }
  switch(whence){
    case SEEK_SET:
      if(offset < 0){
        printf("offset < 0!\n");
        return -1;
      }
      p[fd].disk_ptr = offset;
      ret = offset;
      break;

    case SEEK_CUR:
      p[fd].disk_ptr += offset;
      ret = p[fd].disk_ptr;
      break;

    case SEEK_END:
      p[fd].disk_ptr = p[fd].size + offset;
      ret = p[fd].disk_ptr;

      break;
  }
  return ret;
}


uintptr_t sys_write(uintptr_t fd,uintptr_t buf,uintptr_t len){
  uintptr_t ret;
  if(fd == FD_STDOUT || fd == FD_STDERR){
    ret = p[fd].write((void *)buf,0,len);
  } else{

    if(len + p[fd].disk_ptr > p[fd].size){
      len = p[fd].size - p[fd].disk_ptr;
    }

    uintptr_t offset = p[fd].disk_offset + p[fd].disk_ptr;
    ret = ramdisk_write((void *)buf, offset, len);
    p[fd].disk_ptr += len;
  }
  return ret;
}