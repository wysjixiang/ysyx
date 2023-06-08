
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

static int is_batch_mode = false;


// import
int cpu_exec(int n);
void gpr_display();
void mem_display(int index);
void display_gpr();

char *readline(const char *prompt);


/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  //if (line_read && *line_read) {
  //  add_history(line_read);
  //}

  return line_read;
}


static int cmd_help(char *args);

static int cmd_c(char *args) {
  int ret = 0;
  ret = cpu_exec(-1);     //since cpu_exec( uint64 n), so if use -1(int) as argument, the real execute steps will
                    // be large! this is why you just put 'c' once in NEMU conmand line but the whole 
                    // program ended
                    // single step running
  return ret;
}

static int cmd_q(char *args) {
  return -1;
}

// add more commands functions
static int cmd_s(char *args) {
  if(NULL == args) {
    //printf("s default step forward 1\n");
    return (cpu_exec(1)); // default: single step
  } else{
    int len_args = strlen(args);
    uint64_t n_step = 0;
    int value = 0;
    int i=0;
    while(i < len_args) {
      value = args[i] - '0';
      //printf("value = %d\n",value);
      if(value > 9 || value < 0) { 
        printf("Si arguments error, it must be numbers!\n");
        return 1;
      }
      n_step = n_step * 10 + value;
      i++;
    }
    if(n_step == 0) {
      printf("Si arguments = 0. Please retype and make sure steps bigger than 0");
      return 1;
    } else{
      //printf("Step forward %ld \n",n_step);
      return cpu_exec(n_step);
    }
  }
}

static int cmd_gpr(char *args) {
    gpr_display();
    return 0;
}


static int cmd_mem(char *args) {
    int len_args = strlen(args);
    uint64_t n_step = 0;
    int value = 0;
    int i=0;
    while(i < len_args) {

      if(args[i] >= '0' && args[i] <= '9'){
        value = args[i] - '0';
      } else if(args[i] >= 'a' && args[i] <= 'f'){
        value = args[i] - 'a' + 10;
      } else {
        printf("Si arguments error, it must be numbers!\n");
        return 1;
      }
      n_step = n_step * 16 + value;
      i++;
    }
    int index = (n_step-0x80000000)/8;
    mem_display(index);
    return 0;
}

static int cmd_compare(char *args) {
    display_gpr();
    return 0;
}


static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program",               cmd_c },
  { "q", "Exit NEMU",                                           cmd_q },

  /* TODO: Add more commands */
    { "s", "N steps forward. Add argument N after si with interval blank" , cmd_s },
    { "gpr", "Print gpr " ,                                      cmd_gpr },
    { "mem", "Print mem" ,                                       cmd_mem },
    { "comp", "compare gpr of dut & ref" ,                       cmd_compare },
};

// calculate the length of an array
#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}


int sdb_mainloop() {
  if (is_batch_mode) {  // is_batch_mode = false
    cmd_c(NULL);  
    return 0;
  }
  // read command dead loop
  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    // attention! only the first token!
    char *cmd = strtok(str, " "); // strtok: seperate string in str via delimiter.
                                  // and the delim will be replaced by '\0'
    if (cmd == NULL) { 
      continue; 
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */

    // args point to the following argument
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {  // means only one token and no remaining string
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {       // guess NR_CMD = 3
      if (strcmp(cmd, cmd_table[i].name) == 0) {  //if equ, function return value is 0
        if (cmd_table[i].handler(args) < 0) { // if command is 'q', sdb_mainloop will return directly
             // if quit, change the state to NEMU_QUIT to avoid errors.
          return -1; 
        } 
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
  return 0;
}

void set_batch(){
  is_batch_mode = true;
}