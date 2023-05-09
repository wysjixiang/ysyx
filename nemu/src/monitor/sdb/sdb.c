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
					printf("THe PMem Addr: 0x%08x, value(hex) = 0x%08lx\n", PMem_addr + 4*j, paddr_read(PMem_addr + 4*j,4));
				}

			}

  }

  return 0;
}

static void spaceout(char* input){
	char *p = input;
	char *q = input;

	while('\0' != *p){
		if(' ' != *p){
			*q++ = *p++;
		}	else{
			p++;
		}
	}
	*q = '\0';
}

// take (-x) and (+x) into consideration
#define sign_check_p	(*p == '(' || (*p <= '9' && *p >= '0') || *p == '+'|| *p == '-')
#define sign_check_q	(*q == ')' || (*q <= '9' && *q >= '0') )

static bool check_parentheses(char *p,char *q){
	printf("*p = %c\n*q = %c\n",*p,*q);
	if(*p == '(' && *q == ')'){
		int num = 1;
		p++;
		bool flag_not_paired = 0;
		while(p != q){
			if(*p == '('){
				num++;
			}	else if(*p == ')'){
				num--;
			}
			if(num <0){
				printf("(expression) is invalid, the order of ( or ) is wrong\n");
				assert(0);
			}	else if(num == 0){
				flag_not_paired = 1;
			}
			p++;
		}
		if(num != 1 ){
			printf("(expression) is invalid, the number of () is not paired!\n");
			assert(0);
		}	else if(flag_not_paired){
			// if the bandsides of ( and ) are not paired!
			return false;
		}
		return true;
	}	else if(sign_check_p && sign_check_q){ 
			return false;
	}	else{
		// more than zero bandsides has the invalid bandside expression
		printf("The bandsides of expression is invalid\n");
		assert(0);
	}
}

#define	op_add		1
#define	op_minus	2
#define op_mult		3
#define	op_div		4


static void FindMainOp(char *p_start, char *p_end, char* args){
	if(!(*p_end == ')' || (*p_end >= '0' && *p_end <= '9'))){
		printf("The end of expression is invalid\n");
		assert(0);
	}
	#define	max_len 3
	#define max_op_num 32
	int num =0;	// number of difference of ( and )
	int pre_op = -1;	
	// save previous sign to handle if errors and if mainop
	// -1 means nothing previous
	// 0 means number
	// 1 means + or -
	// 2 means * or /
	// 3 means (
	// 4 means )
	char *p = p_start;
	char *q = p_end;
	char pos[3][max_op_num];
	// pos[0][]	save the sign
	// pos[1][] represents whether it is binocular or unary operator. if binocular, value = 1
	// pos[2][]	save the offset from p_start
	int index = 0;

// while loop for token recognization and unary operator
	while(p != q){
		if(num > 0 || *p == '(' || *p == ')'){
			pre_op = 4;
			if(*p == '('){
				num++;
			}	else if(*p == ')'){
				num--;
			}
		}	else{
			switch(*p){
				case '+':
					if(pre_op == -1 || pre_op == 1 || pre_op == 3){
						pos[0][index] = op_add;
						pos[1][index] = 0;
						pos[2][index++] = p - p_start;
					}	else if(pre_op == 0 || pre_op == 4){
						pos[0][index] = op_add;
						pos[1][index] = 1;
						pos[2][index++] = p - p_start;
					}	else{
						pos[0][index] = op_add;
						pos[1][index] = 0;
						pos[2][index++] = p - p_start;
					}	
					pre_op = 1;
					break;
				
				case '-':
					if(pre_op == -1 || pre_op == 1 || pre_op == 3){
						pos[0][index] = op_minus;
						pos[1][index] = 0;
						pos[2][index++] = p - p_start;
					}	else if(pre_op == 0 || pre_op == 4){
						pos[0][index] = op_minus;
						pos[1][index] = 1;
						pos[2][index++] = p - p_start;
					}	else{
						pos[0][index] = op_minus;
						pos[1][index] = 0;
						pos[2][index++] = p - p_start;
					}	
					pre_op = 1;
					break;

				case '*':
					if(pre_op == 0 || pre_op == 4 ){
							pos[0][index] = op_mult;
							pos[1][index] = 1;
							pos[2][index++] = p - p_start;
					}	else{
						printf("* or / format wrong\n");
						assert(0);
					}
					pre_op = 2;
					break;

				case '/':
					if(pre_op == 0 || pre_op == 4 ){
							pos[0][index] = op_div;
							pos[1][index] = 1;
							pos[2][index++] = p - p_start;
					}	else{
						printf("* or / format wrong\n");
						assert(0);
					}
					pre_op = 2;
					break;
				
				default:
					if(*p >= '0' && *p <= '9'){
						pre_op = 0;
					}	else{
						printf("Unknown character in expression\n");
						assert(0);
					}
					break;
			}
		}
		if(num < 0 ){
			printf("The order of ( and ) is wrong\n");
			assert(0);
		}
		p++;
	}
// after while loop

// check if the expression is legal
	num -= (*q == ')');
	if(num != 0 ){
		printf("The number of ( and ) is not paired\n");
		assert(0);
	}

// check if index ==0, if yes, then this expression is a number
	if(index == 0){
		args[0] = 0;
		args[1] = 0;
		args[2] = 1;
		return;
	}	else{
	// find the MainOp
		uint8_t flag_binocular = 0;
		int j = index;

		while(j--){
			if(pos[1][j]){
				if(pos[0][j] < 3){
					args[0] = pos[2][j];
					args[1] = pos[0][j];
					args[2] = pos[1][j];
					return;
				} else if(!flag_binocular){
					args[0] = pos[2][j];
					args[1] = pos[0][j];
					args[2] = pos[1][j];
				}
				flag_binocular = 1;
			}
		}
		// check if there is to be a MainOp. if no MainOp, this expression should be a number with prefix of + or - or combination of them
		// this is a special case like ++++----+---++++ number
		// obviously, the number is at the end of expression or this expression would beconsidered as an invalid one
		// so the number's address is the pos[2][index-1]+1, the sign should be calculate from via unary operators
		if(flag_binocular){
			return;
		}	else{
			// no MainOp
			int sign_pos = 1;
			j = 0;
			while(j < index){
				if(pos[0][j] == op_minus){
					sign_pos = !sign_pos;
				}
				j++;
			}
			args[0] = pos[2][index-1] + 1;	
			args[1] = 0;
			args[2] = sign_pos;
			return;
		}


	}
	#undef	max_len
}


static int32_t eval(char* p, char* q){
	if(p > q){
		//bad expression
		printf("Invalid Expression\n");
		assert(0);
	} else if(p == q){
		if(*q >= '0' && *q <= '9'){
			return *q -'0';
		}	else{
			printf("Invalid non-number value\n");
			assert(0);
		}
	}	else if(check_parentheses(p,q) == true){
		return eval(p+1,q-1);
	}	else{
			char args[3];
			/*
			args[0] -> offset form p;
			args[1] -> represents number or opcode; 0-number,1-add,2-minus,3-mult,4-div
			args[2] -> sign for number.1-positive 0-negetive
			*/
			//find main op
			FindMainOp(p,q,args);
			printf("offset = %d\n",args[0]);
			printf("opcode = %d\n",args[1]);
			printf("sign = %d\n",args[2]);
			char *p0 = p + args[0];
			// analyze args
			// if expression is a number with prefix of + and -
			if(args[1] == 0){
				int32_t result = 0;
				while(p0 <= q){
					result = result * 10 + *p0++ - '0';
				}
				if(args[2]) {
					return result;
				}	else return (-result);
			}	else{
				switch(args[1]){
					case op_add:
						return eval(p,p0-1) + eval(p0+1,q);

					case op_minus:
						return eval(p,p0-1) - eval(p0+1,q);

					case op_mult:
						return eval(p,p0-1) * eval(p0+1,q);

					case op_div:
						int32_t val = eval(p0+1,q);
						if(val == 0){
							printf("Can not div 0\n");
							assert(0);
						}
						return eval(p,p0-1) / val;
					default:
						printf("args[1] errors\n");
						assert(0);
				}
			}
	}
}


static int cmd_p(char *args){
  if(NULL == args)  {
    printf("CMD_P:No valid arguments. Please retype\n");
    return 1;
  } 
	spaceout(args);
	char *p = args;
	char *q = p + strlen(p) - 1;
	printf("Expression = %d\n",eval(p,q));
	return	0;
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
	{"p", "Caculate Expression Value" ,														cmd_p},
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
