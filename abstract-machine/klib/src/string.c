#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while(*s++!= '\0'){
    len++;
  }
  return len;
}

char *strcpy(char *dst, const char *src) {
  char *p = dst;
  while(*src!= '\0'){
    *dst++ = *src++;
  }
  *dst = '\0';
  return p;
}

char *strncpy(char *dst, const char *src, size_t n) {
  char *p = dst;
  size_t i=0;
  for(; i<n && *src!= '\0' ; i++){
    *dst++ = *src++;
  }
  for(; i < n; i++){
    *dst++ = '\0';
  }
  return p;
}

char *strcat(char *dst, const char *src) {
  size_t len = strlen(dst);
  while(*src != '\0'){
    dst[len++] = *src++;
  }
  dst[len] = '\0';
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  size_t i=0;
  while(s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0'){
    i++;
  }
  if(s1[i] == s2[i]){
    return 0;
  } else{
    if(s1[i] != '\0' && s2[i] != '\0'){
      if(s1[i] > s2[i]){
        return 1;
      } else{
        return -1;
      }
    } else if(s1[i] == '\0'){
      return -1;
    } else{
      return 1;
    } 
  }
}

int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i=0;
  while(s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0' && i < n){
    i++;
    if(i == n){
      return 0;
    }
  }
  if(s1[i] == s2[i]){
    return 0;
  } else{
    if(s1[i] != '\0' && s2[i] != '\0'){
      if(s1[i] > s2[i]){
        return 1;
      } else{
        return -1;
      }
    } else if(s1[i] == '\0'){
      return -1;
    } else{
      return 1;
    } 
  }
}

// why arg c is a int type in memset func?
// Interesting question!
void *memset(void *s, int c, size_t n) {
  char *p = s;
  while(n--){
    *p++ = c;
  }
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  
  if (dst < src) {
    while (n--) {
      *(uint8_t *)dst++ = *(uint8_t *)src++;
    }
  } else if (dst > src) {
    dst += n;
    src += n;
    while (n--) {
      *(uint8_t *)--dst = *(uint8_t *)--src;
    }
  }

  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  size_t i=0;
  char *buf_out = (char *)out;
  char *buf_in = (char *)in;
  for(;i<n;i++){
    buf_out[i] = buf_in[i];
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  if(n == 0){
    return 0;
  }
  size_t i=0;
  char *p1 = (char *)s1;
  char *p2 = (char *)s2;
  while(p1[i] == p2[i]){
    i++;
    if(i== n){
      return 0;
    }
  }
  if(p1[i] > p2[i]){
    return 1;
  } else {
    return -1;
  }
}

#endif
