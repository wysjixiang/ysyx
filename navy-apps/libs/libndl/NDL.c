#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>


static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static uint32_t sys_w = 0, sys_h = 0;
static int x0 = 0, y0 = 0;

static int dev_fd = 0;
static int disp_fd = 0;
static int fb_fd = 0;


void get_all_fd();
void get_dev_fd();
void get_disp_fd();
void get_fb_fd();
void get_screen_wh(int *w, int *h);
void get_syscanvas(uint32_t *Sys_w, uint32_t *Sys_h);

uint32_t NDL_GetTicks() {
	struct timeval tv;
	gettimeofday(&tv,NULL);
  return (tv.tv_sec * 1000 + tv.tv_usec /1000);
}

int NDL_PollEvent(char *buf, int len) {
  return read(dev_fd,buf,len);
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);  // %d - 4 bytes
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  // add
  else {
    if(*w >= sys_w || *w == 0) {
      screen_w = sys_w;
    } else {
      screen_w = *w;
    }
    if(*h >= sys_h || *h ==0) {
      screen_h = sys_h;
    } else {
      screen_h = *h;
    }

    *w = screen_w;
    *h = screen_h;

    x0 = (sys_w - screen_w)/2;
    y0 = (sys_h - screen_h)/2;

  }
  printf("width:%d\n",screen_w);
  printf("height:%d\n",screen_h);
}

// since if we not change the write way, the efficiency is so low.
// so I change the work style.
// first lseek to (x+x0,y+y0)
// then write w as len
// last write h as len
// after this process, write function in nanos know all the info to write to nemu frame buf
// in fact, we have not yet give rom for frambe buffer at nanos since I dont thing it is necessary
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {

  /*
  int bytes = 4;
  long offset = (y0 + y)*sys_w + (x0 + x);
  lseek(fb_fd,offset * bytes,SEEK_SET);
  */
  uint8_t *p = (uint8_t *)pixels;

  if(w == 0) w = screen_w;
  if(h == 0) h = screen_h;

  write(fb_fd,(void *)p,w);
  write(fb_fd,(void *)p,h);
  write(fb_fd,(void *)p,x0 + x);
  write(fb_fd,(void *)p,y0 + y);

}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  
  // remember first get all fd
  get_all_fd();

  get_syscanvas(&sys_w, &sys_h);

  return 0;
}

void NDL_Quit() {
}


void get_syscanvas(uint32_t *Sys_w, uint32_t *Sys_h){
    uint32_t data[2];
    read(disp_fd,data,0); // get w and h
    *Sys_w = data[0];
    *Sys_h = data[1];
}


void get_all_fd(){
  get_dev_fd();
  get_disp_fd();
  get_fb_fd();
}

void get_dev_fd(){
  dev_fd = open("/dev/events","r");
}

void get_disp_fd(){
  disp_fd = open("/proc/dispinfo","r");
}

void get_fb_fd(){
  fb_fd = open("/dev/fb","r");
}


void get_screen_wh(int *w, int *h){
  *w = screen_w;
  *h = screen_h;
}