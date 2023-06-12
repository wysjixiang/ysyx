#include <fs.h>
#include "syscall.h"



void get_table_head(Finfo *table);

size_t std_read(void *buf, size_t offset, size_t len) {
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
  [FD_STDIN]  = {"stdin", 0, 0,  std_read,  std_write, 0},
  [FD_STDOUT] = {"stdout", 0, 0, std_read, std_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, std_read, std_write, 0},
#include "files.h"  // WTF!! WE can use #include like this!!!! 
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  for(int i=0; i< sizeof(file_table)/sizeof(file_table[0]); i++){
    file_table[i].disk_ptr = 0;
    printf("NO.%d offset = %lx\n",i,file_table[i].disk_offset);
  }

  get_table_head(file_table);
  
}

int get_table_num(){
  return sizeof(file_table)/sizeof(file_table[0]);
}
