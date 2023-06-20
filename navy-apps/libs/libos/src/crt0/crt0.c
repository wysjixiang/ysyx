#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  char *empty[] =  {NULL };
  environ = empty;

  int argc = 0;
  char **argv = (char **)(*args);

  /*
  if(*args == 0){
    argv = NULL;
  } else{
    argv = (char **)(*args);
    printf("args = %lx\n",args);
    printf("*argv[] = %lx\n",argv);
    printf("string = %s\n",argv[0]);
    printf("string = %s\n",argv[1]);
    while(1);
  }
  */


  // exit function executes after main and the ret of main is arg of exit!!
  exit(main(argc,argv,0));  
  assert(0);
}
