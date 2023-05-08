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
              // TODO();
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
  } else{
      char *args_1 = strtok(NULL, " ");
      int len_args = strlen(args_1);
      uint64_t n_step = 0;
      int value = 0;
      int i=0;
      while(i < len_args) {
        value = args[i] - '0';
        //printf("value = %d\n",value);
        if(value > 9 || value < 0) { 
          printf("CMD_X arguments 1 error, it must be numbers!\n");
          return 1;
        }
        n_step = n_step * 10 + value;
        i++;
      }

      char * args_2 = strtok(NULL, " ");
      if(NULL == args_2) {
        printf("CMD_x arguments 2 lost\n");
        return 1;
      }
      
      len_args = strlen(args_2);
			int PMem_addr = 0;
      value = 0;
			i = 2;
			//printf("arg1 = %c, arg2 = %c", args_2[0],args_2[1]);
      if(args_2[0] != '0' || (args_2[1] != 'x' && args_2[1] !='X')) {
				printf("CMD_x: Argument 2 format wrong. Please input with the begining of '0x' or '0X'");
				return 1;
      }
			
			while(i < len_args){
				if(args_2[i] >= '0' || args_2[i] <= '9'){
					value = args_2[i] - '0';
				}	else if(args_2[i] >= 'a' || args_2[i] <= 'f'){
					value = args_2[i] - 87;
				}	else if(args_2[i] >= 'A' || args_2[i] <= 'F'){
					value = args_2[i] - 55;
				} else {
					printf("CMD_x: Argument 2 must be a hex format number.");
					return 1;
				}
				PMem_addr = PMem_addr * 16 + value;
        i++;
			}

			if(PMem_addr < 0x80000000 || 0 != (PMem_addr % 4)) // if out of mem or not 4K aligned
			{
				printf("THe PMem_addr out of band or not 4k aligned\n");
        return 1;
			}
      if(n_step == 0) {
        printf("CMD_X arguments = 0. Only print out first mem_addr value");
        return 1;
      } else {
				for(int j=0;j< n_step; j++){
					printf("THe PMem Addr: 0x%x, value = %ld\n", PMem_addr + 4*j, paddr_read(PMem_addr + 4*j,4));
				}

			}

  }

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
  { "si", "N steps forward. Add argument N after si with interval blank" , cmd_si },
  { "info", "Print gpr or watch point status" ,                 cmd_info },
  { "x", "Print mem value",                                     cmd_x },

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
