#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static uint8_t KeyBoardState[83] = {0};


int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {

  char buf[64];
  int a = 0;
  a = NDL_PollEvent(buf, 64);
  if(a == 0) return 0;

  if(buf[0] == 'k' && buf[1] == 'd'){
    ev->type = SDL_KEYDOWN;
  } else{
    ev->type = SDL_KEYUP;
  }

  for(int i=0;i< 83;i++){
    if(strcmp(keyname[i],buf+3) == 0){
      ev->key.keysym.sym = i;
      KeyBoardState[i] = !ev->type;
    }
  }
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {

  // dead loop for event
  while(!SDL_PollEvent(event));

  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return KeyBoardState;
}
