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

//sys_function
uintptr_t sys_open(uintptr_t name);
uintptr_t sys_close(uintptr_t fd);
uintptr_t sys_read(uintptr_t fd,uintptr_t buf,uintptr_t len);
uintptr_t sys_lseek(uintptr_t fd,uintptr_t offset,uintptr_t whence);
uintptr_t sys_write(uintptr_t fd,uintptr_t buf,uintptr_t len);
int sys_gettimeofday(uintptr_t tv);

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
      c->GPR_a0 = sys_close(a[0]);
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

    case SYS_gettimeofday:
      c->GPR_a0 = sys_gettimeofday(a[0]);
      break;

    default: panic("Unhandled syscall ID = %d", a[3]);
  }
}


uintptr_t sys_open(uintptr_t name){

  int num = get_table_num();
  char *_name = (char *)name;
  for(int i=0;i<num;i++){
    if(strcmp(_name,p[i].name) == 0){
      printf("filename = %s open!\n",_name);
      return i;
    }
  }

  printf("File not found!\n");
  assert(0);
  return -1;
}


uintptr_t sys_close(uintptr_t fd){

  printf("filename = %s closed!\n",p[fd].name);
  p[fd].disk_ptr = 0;
  return 0;
}

uintptr_t sys_read(uintptr_t fd,uintptr_t buf,uintptr_t len){
  uintptr_t ret;
  if(fd < FD_NUM){
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

uintptr_t sys_write(uintptr_t fd,uintptr_t buf,uintptr_t len){
  uintptr_t ret;
  if(fd < FD_NUM){
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

uintptr_t sys_lseek(uintptr_t fd,uintptr_t offset,uintptr_t whence){

  uintptr_t ret = -1;
  intptr_t _offset = (intptr_t)offset;
  int num = get_table_num();
  if(fd >= num){
    printf("fd wrong!\n");
    assert(0);
  }
  switch(whence){
    case SEEK_SET:
      if(_offset < 0){
        printf("_offset < 0!\n");
        return -1;
      }
      p[fd].disk_ptr = _offset;
      ret = _offset;
      break;

    case SEEK_CUR:
      p[fd].disk_ptr += _offset;
      if(p[fd].disk_ptr > p[fd].size){
        printf("SEKK_CUR Overstep, offset = %d\n",_offset);
        p[fd].disk_ptr -= _offset;
        return -1;
      }
      ret = p[fd].disk_ptr;
      break;

    case SEEK_END:
      if(_offset > 0 || p[fd].disk_ptr + _offset < 0){
        printf("SEEK_END Overstep\n");
        return -1;
      }
      p[fd].disk_ptr = p[fd].size + _offset;
      ret = p[fd].disk_ptr;

      break;
  }
  return ret;
}



      
int sys_gettimeofday(uintptr_t tv){

  uint64_t *p = (uint64_t *)tv;
  uint64_t sec;
  uint64_t us;
  uint64_t num = io_read(AM_TIMER_UPTIME).us;
  sec = num / 1000000;
  us = num - sec*100000;
  p[0] = sec;
  p[1] = us;
  return 0;
}