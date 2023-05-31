#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)
// add
#define AUDIO_SBUF_END_ADDR     (AUDIO_ADDR + 0x1c)
#define CONFIG_SB_ADDR 0xa1200000

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  uint32_t exist = inl(AUDIO_INIT_ADDR);
  cfg->present = exist;
}

// config the audio conf
void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR,ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR,ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR,ctrl->samples);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  // need to know the buffer pointer
  uint32_t *data = (uint32_t *)ctl->buf.start;
  uint32_t sbuf_end = inl(AUDIO_SBUF_END_ADDR);
  uint32_t len = ctl->buf.end - ctl->buf.start;
  uint32_t sbufsize = inl(AUDIO_SBUF_SIZE_ADDR);
  while(len + inl(AUDIO_COUNT_ADDR) > sbufsize);
  if(sbuf_end + len <= sbufsize){
    int div = len/4;
    int rem = len %4;
    for(int i=0;i<div;i++){
      outl(sbuf_end + CONFIG_SB_ADDR,*data++);
      sbuf_end += 4;
    }
    for(int i=0;i<rem;i++){
      outb(CONFIG_SB_ADDR + sbuf_end++,*(uint8_t *)data++);
    }
    if(sbuf_end > sbufsize){
      sbuf_end = 0;
    }
  } else{
    int distance2end = sbufsize - sbuf_end;
    int distancezero = len - distance2end;

    int div1 = distance2end / 4;
    int rem1 = distance2end % 4;
    for(int i=0;i<div1;i++){
      outl(sbuf_end + CONFIG_SB_ADDR ,*data++);
      sbuf_end += 4;
    }
    for(int i=0;i<rem1;i++){
      outb(CONFIG_SB_ADDR + sbuf_end++,*(uint8_t *)data++);
    }
    sbuf_end = 0;

    int div2 = distancezero / 4;
    int rem2 = distancezero % 4;
    for(int i=0;i<div2;i++){
      outl(sbuf_end + CONFIG_SB_ADDR  ,*data++);
      sbuf_end += 4;
    }
    for(int i=0;i<rem2;i++){
      outb(CONFIG_SB_ADDR + sbuf_end++,*(uint8_t *)data++);
    }
  }
  outl(AUDIO_SBUF_END_ADDR,sbuf_end);
  outl(AUDIO_COUNT_ADDR,inl(AUDIO_COUNT_ADDR) + len);
}
