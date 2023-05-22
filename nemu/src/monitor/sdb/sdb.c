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

#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
// add include file
#include <memory/paddr.h>


static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);     //since cpu_exec( uint64 n), so if use -1(int) as argument, the real execute steps will
                    // be large! this is why you just put 'c' once in NEMU conmand line but the whole 
                    // program ended
                    // single step running
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);


// add more commands functions
static int cmd_si(char *args) {
  if(NULL == args) {
    //printf("si default step forward 1\n");
    cpu_exec(1); // default: single step
    return 0;
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
      return 0;
    } else{
      //printf("Step forward %ld \n",n_step);
      cpu_exec(n_step);
      return 0;
    }
  }
}


static int cmd_info(char* args) {
  if(NULL == args)  {
    printf("CMD_INFO: No valid arguments. Please retype\n");
    return 1;
  } else {
      int len = strlen(args);
      if(1 != len) {
        printf("Length of arguments = 1. Valid: 'r' or 'w'\n");
        return 1;
      } else  {
          switch(*args){
            case  'r':
              isa_reg_display();
              break;
            case  'w':
              ShowWp();
              break;
            default: 
              printf("Invalid arguments. Valid: 'r' or 'w'\n");
              break;
          }
      }
    return 0;
  }

}


static int cmd_x(char *args)  {
  if(NULL == args)  {
    printf("CMD_X:No valid arguments. Please retype\n");
    return 1;
  }
  char *p = strtok(NULL," ");
  bool success = 0;
  word_t num = expr(p,&success);
  if(success == 0){
    printf("CMD_X: arguments 1 error\n");
    return 1;
  }
  word_t addr = 0;
  p = args + strlen(p) +1;
  addr = expr(p,&success);
  
  for(int i=0;i<num;i++){
    printf("Addr: 0x%08lx, Value: 0x%016lx\n",addr + 8*i, paddr_read(addr + 8*i,8));
  }
  return 0;
}

// static void spaceout(char* input){
// 	char *p = input;
// 	char *q = input;

// 	while('\0' != *p){
// 		if(' ' != *p){
// 			*q++ = *p++;
// 		}	else{
// 			p++;
// 		}
// 	}
// 	*q = '\0';
// }

int cmd_p(char *args){
  if(NULL == args)  {
    printf("CMD_P:No valid arguments. Please retype\n");
    return 1;
  }	
	uint64_t result;
	bool success = 0;
	result = expr(args,&success);
	printf("result = 0x%016lx\n",result);
	if(success) return 0;
	else return 1;

}


static int cmd_w(char *args){
	bool ret = 0;
	ret = AddWp(args);
	if(ret == 0){
		printf("Add wp failed\n");
		return 1;
	}
	else return 0;

}


// pay attention args is a str that represents a number points to the wp id
static int cmd_d(char *args){
	bool ret = 0;
	ret = DelWp(args);
	if(ret == 0){
		printf("Del wp failed\n");
		return 1;
	}
	else return 0;

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
  { "si", "N steps forward. Add argument N after si with interval blank" , cmd_si },
  { "info", "Print gpr or watch point status" ,                 cmd_info },
  { "x", "Print mem value",                                     cmd_x },
	{	"p", "Caculate Expression Value" ,													cmd_p},
	{	"w", "Sets Watch Point" ,																		cmd_w},
	{	"d", "Delete Watch Point" ,																	cmd_d},
};

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

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {  // is_batch_mode = false
    cmd_c(NULL);  
    return;
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
          nemu_state.state = NEMU_QUIT;   // if quit, change the state to NEMU_QUIT to avoid errors.
          return; 
        } 
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
