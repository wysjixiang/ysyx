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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();


extern int64_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif
	
	// gen-expr check
//#define exper_check
#ifdef exper_check
	FILE *file = fopen("/home/jixiang/ysyx-workbench/nemu/log", "r");
	if(!file){
		printf("Can not open the file\n");
		assert(0);
	}
	#define max_len 65535
	char buf[max_len] ;
	int cnt =0;
	int run_cnt = 0;
	while(fgets(buf,max_len,file) != NULL){
		printf("run_loop = %d\n",run_cnt++);

//#define TEST
#ifdef TEST
	if(run_cnt < TEST_NUM) continue;

#endif


		char *value = strtok(buf," ");
		// get value
		int len = strlen(value);
		int64_t result =0;
		bool flag = 1;
			switch(value[0]){
				case '+':
					flag =1;
					break;

				case '-':	// '-'
					flag = 0;
					break;

				default:
					result = value[0] - '0';
					break;
			}
		for(int i=1; i<len;i++){
			result = result*10 + value[i] - '0';
		}
		if(flag == 0) result = result * (-1);
		char *expression = buf + len + 1 ;
		int exp_len = strlen(expression);
		// make Newline sign change to  end of line;
		expression[exp_len-1] = '\0';

		bool ok = 0;
		int64_t cal = expr(expression,&ok);
		if(result != cal){
			printf("Value mismatched cnt: %d!\nThe correct: %lu\nThe Calculated: %lu\n",cnt++,result,cal);
			assert(0);
		}	else{
			;
		}
	}
	return 0;

#undef exper_check
#endif
//

  /* Start engine. */
  engine_start();

  printf("Quit NEMU!\n");
  return is_exit_status_bad();

}
