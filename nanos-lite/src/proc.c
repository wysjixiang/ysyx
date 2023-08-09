#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void naive_uload(PCB *pcb, const char *filename);
Context *schedule(Context *prev);
uintptr_t loader(PCB *pcb, const char *filename);
void *new_page(size_t nr_page);

void switch_boot_pcb()
{
  current = &pcb_boot;
}

void hello_fun(void *arg)
{
  int j = 1;
  while (1)
  {
    //Log("Hello World from Nanos-lite with arg '0x%lx' for the %dth time!", (uintptr_t)arg, j);
    printf("Hello World from Nanos-lite with arg '0x%lx' for the %dth time!\n", (uintptr_t)arg, j);
    j++;
    //yield();
  }
}

void test(void *arg)
{
  while (1)
  {
    printf("Test!\n");
  }
}

void context_uload(PCB *_pcb, const char *filename, char *argv[], char *envp[])
{

  protect(&_pcb->as);
  // now we get the user virtual area

  // stack space
  Area stack;
  stack.end = _pcb->as.area.end;
  stack.start = stack.end - 8 * PGSIZE; //32 KB

  // before loader, you need to copy argv and envp args;
  static char arg0[64];
  static char arg1[64];
  static char *pp[2];
  if (argv == NULL || argv[0] == NULL || argv[1] == NULL)
  {

    pp[0] = NULL;
    pp[1] = NULL;
  }
  else
  {
    strcpy(arg0, argv[0]);
    strcpy(arg1, argv[1]);
    pp[0] = arg0;
    pp[1] = arg1;

    printf("name = %s\n", pp[0]);
    printf("name = %s\n", pp[1]);
  }
  uintptr_t _entry = loader(_pcb, filename);
  _pcb->cp = ucontext(&_pcb->as, stack, (void *)_entry, pp, envp);
}

void context_kload(PCB *_pcb, void (*entry)(void *), void *arg)
{


  Area kstack;
  kstack.start = (void *)_pcb;
  kstack.end = (void *)_pcb + sizeof(PCB);



/*
  uintptr_t pcb_begin = (uintptr_t)_pcb;
  uintptr_t pcb_end = pcb_begin + sizeof(PCB);
  _pcb->as.area.start = (void *)pcb_begin;
  _pcb->as.area.end = (void *)pcb_end;
*/

  _pcb->cp = kcontext(kstack, entry, arg);
}

void init_proc()
{
  context_kload(&pcb[0], hello_fun, (void *)0x100);
  //context_uload(&pcb[1], "/bin/bird", NULL, NULL);
  context_uload(&pcb[1], "/bin/hello", NULL, NULL);
  context_uload(&pcb[2], "/bin/menu", NULL, NULL);
  // context_kload(&pcb[1], test, 0);
  switch_boot_pcb();
}

/*
void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  naive_uload(NULL,"/bin/pal");
}
*/

Context *schedule(Context *prev)
{
  static uint32_t pool = 0;
  static uint32_t num_pal = 0;
  current->cp = prev;
  current = &pcb[pool % 3];

  if(pool%3 == 2){
    if(num_pal > 10){
      num_pal = 0;
      pool++;
    } else{
      num_pal++;
    }
  } else{
    pool++;
  }

  return current->cp;
}

PCB *get_upcb()
{
  return &pcb[2];
}