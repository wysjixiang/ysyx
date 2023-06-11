#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };
  environ = empty;
  // exit function executes after main and the ret of main is arg of exit!!
  exit(main(0, empty, empty));  
  assert(0);
}
