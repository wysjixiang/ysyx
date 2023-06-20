#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void naive_uload(PCB *pcb, const char *filename);
Context* schedule(Context *prev);
uintptr_t loader(PCB *pcb, const char *filename);
void* new_page(size_t nr_page);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '0x%lx' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void test(void *arg){
  while(1){
    printf("Test!\n");
    yield();
  }
}


void context_uload(PCB *_pcb, const char *filename, char *argv[], char *envp[]){

  uintptr_t pcb_begin = (uintptr_t)_pcb;
  uintptr_t pcb_end = pcb_begin + sizeof(PCB);
  _pcb->as.area.start = (void *)pcb_begin;
  _pcb->as.area.end = (void *)pcb_end;


  uintptr_t _heap_end = (uintptr_t)new_page(8) + PGSIZE*8;

  Context *_p = (Context *)(pcb_end - 36*8);
  _p->GPR_a0 = _heap_end;


  // before loader, you need to copy argv and envp args;
  static char arg0[64];
  static char arg1[64];
  static char *pp[2];
  if(argv == NULL || argv[0] == NULL || argv[1] == NULL){

    pp[0] = NULL;
    pp[1] = NULL;

  } else{
    strcpy(arg0,argv[0]);
    strcpy(arg1,argv[1]);
    pp[0] = arg0;
    pp[1] = arg1;
  
    printf("name = %s\n",pp[0]);
    printf("name = %s\n",pp[1]);
  }


  uintptr_t _entry = loader(NULL, filename);

  _pcb->cp = ucontext(&_pcb->as,_pcb->as.area, (void *)_entry,pp, envp);

}



void context_kload(PCB *_pcb,void (*entry)(void *), void *arg){

  uintptr_t pcb_begin = (uintptr_t)_pcb;
  uintptr_t pcb_end = pcb_begin + sizeof(PCB);
  _pcb->as.area.start = (void *)pcb_begin;
  _pcb->as.area.end = (void *)pcb_end;

  pcb->cp = kcontext(_pcb->as.area, entry, arg);
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void *)0x100);
  context_uload(&pcb[1], "/bin/menu",0,0);
  switch_boot_pcb();
}

/*
void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  naive_uload(NULL,"/bin/menu");
}
*/

Context* schedule(Context *prev) {
  static int pool = 0;
  current->cp = prev;
  current = &pcb[pool%2];
  pool++;

  return current->cp;
}

PCB *get_upcb(){
  return &pcb[1];
}