/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  // add index for stream management
  reg_sbuf_start,
  reg_sbuf_end,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;


// put data to audio stream;
void audio_callback(void *userdata, Uint8 * stream,int len){
  uint32_t sbuf_start = audio_base[reg_sbuf_start];
  int num_buf = audio_base[reg_count];
  uint32_t bufsize = audio_base[reg_sbuf_size];

  #define audio_test
  #ifdef audio_test
  if(num_buf >= len){
    if(sbuf_start + len <= bufsize){
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),len);
      //printf("sbuf_s:%d, sbuf_end:%d\n",sbuf_start,audio_base[reg_sbuf_end]);
      sbuf_start +=len;
    } else{
      int distance2end = bufsize - sbuf_start;
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),distance2end);
      memcpy(stream+distance2end,(uint8_t *)sbuf,len-distance2end);
      sbuf_start = len-distance2end;
    }
    audio_base[reg_count] -= len;
  } else{
    int difzero = len - num_buf;
    if(sbuf_start + num_buf <= bufsize){
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),num_buf);
      memset(stream + num_buf,0,difzero);
      sbuf_start += num_buf;
    } else{
      int dis2end = bufsize - sbuf_start;
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),dis2end);
      memcpy(stream + dis2end,(uint8_t *)sbuf,num_buf - dis2end);
      memset(stream + num_buf,0,difzero);
      sbuf_start = num_buf-dis2end;
    }
  
    audio_base[reg_count] = 0;
  }
  #else
  if(num_buf >= len){
    if(sbuf_start + len <= bufsize){
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),len);
      sbuf_start +=len;
    } else{
      int distance2end = bufsize - sbuf_start;
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),distance2end);
      memcpy(stream+distance2end,(uint8_t *)sbuf,len-distance2end);
      sbuf_start = len-distance2end;
    }
    audio_base[reg_count] -= len;
  } else{
    int difzero = len - num_buf;
    if(sbuf_start + num_buf <= bufsize){
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),num_buf);
      memset(stream,0,difzero);
      sbuf_start += num_buf;
    } else{
      int dis2end = bufsize - sbuf_start;
      memcpy(stream,(uint8_t *)(sbuf+sbuf_start),dis2end);
      memcpy(stream,(uint8_t *)sbuf,num_buf - dis2end);
      memset(stream,0,difzero);
      sbuf_start = num_buf-dis2end;
    }
    audio_base[reg_count] = 0;
  }
  #endif
  audio_base[reg_sbuf_start] = sbuf_start;
  SDL_PauseAudio(0);
}


static void audio_io_handler(uint32_t offset, int len, bool is_write) {

  if(offset == 8){
    SDL_AudioSpec s = {};
    SDL_memset(&s, 0, sizeof(s)); /* or SDL_zero(s) */
    s.format = AUDIO_S16SYS;  // 假设系统中音频数据的格式总是使用16位有符号数来表示
    s.userdata = NULL;        // 不使用

    s.silence = 0;
    s.freq = audio_base[0];
    s.channels = audio_base[1];             /**< Number of channels: 1 mono, 2 stereo */
    s.samples = audio_base[2]/s.channels;             /**< Audio buffer size in sample FRAMES (total samples divided by channel count) */
                    /**< Audio buffer size in bytes (calculated) */
    s.callback = audio_callback; /**< Callback that feeds the audio device (NULL to use SDL_QueueAudio()). */

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_OpenAudio(&s, NULL);
    // unpause
    printf("fre:%d,channels:%d,samples:%d,silence:%d\n",s.freq,s.channels,s.samples,s.silence);
    SDL_PauseAudio(0);
  }
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;  //24 Bytes
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
  
  // adding codes
  audio_base[reg_sbuf_size] = 0x10000;
  audio_base[reg_init] = true;
  audio_base[reg_count] = 0;
  audio_base[reg_sbuf_start] = 0;
  audio_base[reg_sbuf_end] = 0;
}
