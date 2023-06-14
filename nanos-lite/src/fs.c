#include <fs.h>
#include "syscall.h"


size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
void get_cpu_config(uint32_t *w,uint32_t *h);

void get_table_head(Finfo *table);
Finfo* trans_table_head();

size_t read_error(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t std_write(const void *buf, size_t offset, size_t len) {
  uint8_t *_buf = (uint8_t *)buf;
  size_t ret = len;
  while(len--){
    putch(*_buf++);
  }
  return ret;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0,  read_error,  0, 0},
  [FD_STDOUT] = {"stdout", 0, 0, read_error, serial_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, read_error, serial_write, 0},
  [FD_EVENTS] = {"/dev/events", 0 ,0 , events_read, 0, 0},  // dev/events. read only
  [FD_DISP] = {"/proc/dispinfo", 0 ,0 , dispinfo_read, 0, 0},  // proc/dispinfo. read only
  [FD_FB] = {"/dev/fb", 0, 0, 0, fb_write, 0 },    // video frame. write only
#include "files.h"  // WTF!! WE can use #include like this!!!! 
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  for(int i=0; i< sizeof(file_table)/sizeof(file_table[0]); i++){
    file_table[i].disk_ptr = 0;
    if(i >= FD_NUM){
      file_table[i].read = ramdisk_read;
      file_table[i].write = ramdisk_write;
    }
    printf("NO.%d offset = %lx\n",i,file_table[i].disk_offset);
  }

  get_table_head(file_table);

  uint32_t w_h[2];
  get_cpu_config(&w_h[0],&w_h[1]);
  file_table[FD_FB].size = w_h[0] * w_h[1] * 4; // set size to system screen size
  
}

int get_table_num(){
  return sizeof(file_table)/sizeof(file_table[0]);
}

Finfo* trans_table_head(){
  return file_table;
}