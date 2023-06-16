#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>



void get_screen_wh(int *w, int *h);


// test found that the palettes from src and dst are the same !
void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);

  int dst_x,dst_y;

  int src_x = (srcrect == NULL ? 0 : srcrect->x);
  int src_y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  if(dstrect == NULL){
    dst_x = 0;
    dst_y = 0;
  } else{
    dst_x = dstrect->x;
    dst_y = dstrect->y;
  }
  
  assert(src_x + w <= src->w);
  assert(src_y + h <= src->h);
  assert(dst_x + w <= dst->w);
  assert(dst_y + h <= dst->h);

  uint32_t offset_src = src_x + src_y * src->w;
  uint32_t offset_dst = dst_x + dst_y * dst->w;
  uint32_t *p_src = (uint32_t *)src->pixels;
  uint32_t *p_dst = (uint32_t *)dst->pixels;

//#define TEST
#ifdef TEST
  if(src->format->BytesPerPixel == 1){
    uint8_t *p_src_8 = (uint8_t *)src->pixels;
    uint8_t *p_dst_8 = (uint8_t *)dst->pixels;
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
      uint32_t src_pelette = (src->format->palette->colors[p_src_8[offset_src + j]].r << 2*8) |
      (src->format->palette->colors[p_src_8[offset_src + j]].a << 3*8)          |
      (src->format->palette->colors[p_src_8[offset_src + j]].g << 8)          |
      (src->format->palette->colors[p_src_8[offset_src + j]].b );
        for(int i=0;i<256;i++){
          uint32_t dst_pelette = (dst->format->palette->colors[i].r << 2*8) |
          (dst->format->palette->colors[i].a << 3*8)          |
          (dst->format->palette->colors[i].g << 8)          |
          (dst->format->palette->colors[i].b );
          if(dst_pelette == src_pelette){
            p_dst_8[offset_dst + j] = i;
            break;
          }
          if(i == 255){
            printf("error!\n");
            assert(0);
          }
        }
      }
      offset_dst += dst->w;
      offset_src += src->w;
    }
  }
#else
  // if 8 bit pixels
  if(src->format->BytesPerPixel == 1){

    uint8_t *p_src_8 = (uint8_t *)src->pixels;
    uint8_t *p_dst_8 = (uint8_t *)dst->pixels;

    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
        p_dst_8[offset_dst + j] = p_src_8[offset_src + j];
      }
      offset_dst += dst->w;
      offset_src += src->w;
    }
  } 
#endif
  else{
    for(int i=0;i<h;i++){
      for(int j=0;j<w;j++){
        p_dst[offset_dst + j] = p_src[offset_src + j];
      }
      offset_dst += dst->w;
      offset_src += src->w;
    }
  }
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {

  printf("color = %x\n",color);
  // if 8 bits
  if(dst->format->BytesPerPixel == 1){
    uint8_t *p_8 = dst->pixels;
    uint8_t color_index = 0;
    // test
    /*
    for(int i=0;i<dst->format->palette->ncolors;i++){
      if((dst->format->palette->colors[color].r << 2*8) |
        (dst->format->palette->colors[color].g << 8)          |
        (dst->format->palette->colors[color].b ) == color){
          color_index = i;
          break;
        }
    }
    */
    for(int i=0;i<dst->format->palette->ncolors;i++){
      if( dst->format->palette->colors[i].val == color){
          color_index = i;
          printf("Found it!\n");
          break;
        }
    }
    //
    //color = (dst->format->palette->colors[color].r << 2*8) |
    //(dst->format->palette->colors[color].g << 8)          |
    //(dst->format->palette->colors[color].b );

    if(dstrect != NULL){
      int offset_8 = dstrect->x + dstrect->y * dst->w;
      for(int i=0;i<dstrect->h;i++){
        for(int j=0;j<dstrect->w;j++){
          p_8[offset_8 + j] = color_index;
        }
        offset_8 += dst->w;
      }
    } else{
        for(int i=0;i<dst->h * dst->pitch ;i++){
          p_8[i] = color_index;
        }
    }
  } else{
    uint32_t *p = (uint32_t *)dst->pixels;
    if(dstrect != NULL){
      uint32_t offset = dstrect->x + dstrect->y * dst->w;
      for(int i=0;i<dstrect->h;i++){
        for(int j=0;j<dstrect->w;j++){
          p[offset + j] = color;
        }
        offset += dst->w;
      }
    } else{
      for(int i=0;i<dst->h * dst->pitch /sizeof(uint32_t);i++){
        p[i] = color;
      }
    }

  }
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h) {

  /*
  if(s->format->BitsPerPixel != 32){
    printf("BitPerpixel = %d\n",s->format->BitsPerPixel);
    //assert(0);
  }
  */

  // it's better to set a fixed length buffer for pixels
  // and more reasonable to get the length from SDL_init
  // but ... you know I'm lazy
  static uint32_t p[400*300];

  // if 8 bit pixel

#define TEST1
#ifdef TEST1
  if(s->format->BitsPerPixel == 8){
    int w_8,h_8;
    if(w == 0 || h == 0){
      get_screen_wh(&w_8, &h_8);
    } else{
      w_8 = w;
      h_8 = h;
    }

    uint32_t offset = x + y*s->w;
    int cnt = 0;
    for(int i=0;i<h_8;i++){
      for(int j=0;j<w_8;j++){
        uint32_t pixel_data = 
        (s->format->palette->colors[s->pixels[offset+j]].a << 3*8)       |
        (s->format->palette->colors[s->pixels[offset+j]].r << 2*8)       |
        (s->format->palette->colors[s->pixels[offset+j]].g << 8)         |
        (s->format->palette->colors[s->pixels[offset+j]].b );
        p[cnt++] = pixel_data;
      }
      offset += s->w;
    }
    NDL_DrawRect(p,x, y, w_8, h_8);
  } 

#else
  if(s->format->BitsPerPixel == 8){
    int w_8,h_8;
    if(w == 0 || h == 0){
      get_screen_wh(&w_8, &h_8);
    } else{
      w_8 = w;
      h_8 = h;
    }
    uint8_t *palette = s->pixels;
    for(int i=0;i<w_8*h_8;i++){
      p[i] = (s->format->palette->colors[palette[i]].r << 2*8) |
      (s->format->palette->colors[palette[i]].a << 3*8)          |
      (s->format->palette->colors[palette[i]].g << 8)          |
      (s->format->palette->colors[palette[i]].b )
      ;
    }
    NDL_DrawRect(p,x, y, w_8, h_8);
  } 
#endif
  else{
    NDL_DrawRect((uint32_t *)s->pixels,x, y, w, h);
  }

}

// APIs below are already implemented.

static inline int maskToShift(uint32_t mask) {
  switch (mask) {
    case 0x000000ff: return 0;
    case 0x0000ff00: return 8;
    case 0x00ff0000: return 16;
    case 0xff000000: return 24;
    case 0x00000000: return 24; // hack
    default: assert(0);
  }
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
    uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8) {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  } else {
    s->format->palette = NULL;
    s->format->Rmask = Rmask; s->format->Rshift = maskToShift(Rmask); s->format->Rloss = 0;
    s->format->Gmask = Gmask; s->format->Gshift = maskToShift(Gmask); s->format->Gloss = 0;
    s->format->Bmask = Bmask; s->format->Bshift = maskToShift(Bmask); s->format->Bloss = 0;
    s->format->Amask = Amask; s->format->Ashift = maskToShift(Amask); s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC)) {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
    int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
      Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s) {
  if (s != NULL) {
    if (s->format != NULL) {
      if (s->format->palette != NULL) {
        if (s->format->palette->colors != NULL) free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC)) free(s->pixels);
    free(s);
  }
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
  if (flags & SDL_HWSURFACE) NDL_OpenCanvas(&width, &height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
      DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if(w == dstrect->w && h == dstrect->h) {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else {
    assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if(s->flags & SDL_HWSURFACE) {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i ++) {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }

    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len) {
  int i;
  uint8_t (*pdst)[4] = dst;
  uint8_t (*psrc)[4] = src;
  union {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16) {
#define macro(i) \
    tmp.val32 = *((uint32_t *)psrc[i]); \
    *((uint32_t *)pdst[i]) = tmp.val32; \
    pdst[i][0] = tmp.val8[2]; \
    pdst[i][2] = tmp.val8[0];

    macro(i + 0); macro(i + 1); macro(i + 2); macro(i + 3);
    macro(i + 4); macro(i + 5); macro(i + 6); macro(i + 7);
    macro(i + 8); macro(i + 9); macro(i +10); macro(i +11);
    macro(i +12); macro(i +13); macro(i +14); macro(i +15);
  }

  for (; i < len; i ++) {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags) {
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface* ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
    fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask) p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s) {
  assert(0);
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s) {
  assert(0);
}
