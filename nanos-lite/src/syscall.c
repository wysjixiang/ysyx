#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <proc.h>

// import
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(void const *buf, size_t offset, size_t len);
int get_table_num();
Context* schedule(Context *prev);
void switch_boot_pcb();
PCB *get_upcb();


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
uintptr_t sys_execve(uintptr_t fname, uintptr_t argv, uintptr_t envp);
bool sys_brk(uintptr_t program_break, uintptr_t increment, Context *c);
int sys_gettimeofday(uintptr_t tv);
void naive_uload(PCB *pcb, const char *filename);
void context_uload(PCB *_pcb, const char *filename, char *argv[], char *envp[]);
void* new_page(size_t nr_page);

Context *do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR_a0;
  a[1] = c->GPR_a1;
  a[2] = c->GPR_a2;
  a[3] = c->GPR_a7;

  switch (a[3]) {

    case SYS_exit:
      char *name = "/bin/menu";
      sys_execve((uintptr_t)name,0,0);
      //halt(a[0]);
      break;

    case SYS_yield:
      c = schedule(c);
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
      c->GPR_a0 = sys_brk(a[0],a[1],c);
      break;

    case SYS_gettimeofday:
      c->GPR_a0 = sys_gettimeofday(a[0]);
      break;

    case SYS_execve:
      c->GPR_a0 = sys_execve(a[0],a[1],a[2]);
      break;

    default: panic("Unhandled syscall ID = %d", a[3]);
  }
  return c;
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
  us = num - sec*1000000;
  p[0] = sec;
  p[1] = us;
  return 0;
}


uintptr_t sys_execve(uintptr_t fname, uintptr_t argv, uintptr_t envp){
  
  assert(0);
  context_uload(get_upcb(), (char *)fname,(char **)argv,(char **)envp);
  switch_boot_pcb();
  yield();
  return 0;
}

//static uintptr_t program_ptr = 0;

bool sys_brk(uintptr_t program_break, uintptr_t increment, Context *c){

  AddrSpace _c;
  _c.ptr = c->pdir;

  if(increment == 0){
    return 0;
  }

  void *va = (void *)program_break;

  for(; (uintptr_t)va < program_break + increment;va+=PGSIZE){
    map(&_c,va,NULL, 0);
  }

  return 0;
}

/*
bool sys_brk(uintptr_t program_break, uintptr_t increment, Context *c){

  if(increment == 0 || (program_break + increment <= program_ptr)){
    return 0;
  }

//printf("program_break = %lx, increment = %lx\n",program_break,increment);


  void *va = NULL;
  void *pa = NULL;
  uintptr_t num_pages = 0;

  if(program_ptr == 0){
    num_pages = (increment % PGSIZE == 0) ? increment/PGSIZE : increment/PGSIZE + 1;
    va = (void *)program_break;
  } else{
    if((program_ptr & 0xFFF) == 0){
      num_pages = (increment % PGSIZE == 0) ? increment/PGSIZE : increment/PGSIZE + 1;
      va = (void *)program_ptr;
    } else{
      num_pages = (program_ptr + increment - ((program_ptr >> 12)<<12))/PGSIZE;
      va = (void *)(((program_ptr >> 12)+1)<<12);
    }
  }

  program_ptr = program_break + increment;

  if(num_pages == 0){
    return 0;
  }
  AddrSpace _c;
  _c.ptr = c->pdir;
  pa = new_page(num_pages);
  for(int i=0; i<num_pages; i++){
    map(&_c,va, pa, 0);
    va += PGSIZE;
    pa += PGSIZE;
  }

  return 0;
}
*/