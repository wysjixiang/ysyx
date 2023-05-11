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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"#include <stdint.h>\n"
"int main() { "
"  int64_t result = %s; "
"  printf(\"%%ld\", result); "
"  return 0; "
"}";

static volatile uint32_t buf_index = 0;

static int choose(int num){
	return rand()%num;
}

static void gen_rand_op(){
	int randnum = rand()%4;
	switch(randnum ){
		case 0:
			buf[buf_index++] = '+';
			break;
		case 1:
			buf[buf_index++] = '-';
			break;

		case 2:
			buf[buf_index++] = '*';
			break;

		default:
			buf[buf_index++] = '/';
			break;
	}
}

static void gen(char input){
	buf[buf_index++] = input;
}

static void gen_num(){
	int randnum = rand()%10;
	buf[buf_index++] = randnum + '0';
}


static void gen_rand_expr() {
	if(buf_index >= 65530) return ;
	// randomly insert blankspace, opportunity 
	int randnum = rand()%10;
	if(randnum == 9){
		buf[buf_index++] = ' ';
	}

	switch(choose(3)){
		case 0:
			gen_num();
			break;

		case 1:
			gen('('); gen_rand_expr();gen(')');
			break;

		case 2:
			gen_rand_expr();gen_rand_op();gen_rand_expr();
			break;
	}
}



// we can use '>' to redirect the outputs to other files
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
		buf_index = 0;
    gen_rand_expr();
		buf[buf_index] = '\0';


		// code_format -> %s and %u.
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc -Wall -Werror /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

		// popen() API. FILE *popen(const char *command, const char *mode);
		// execute command and 'r' mode will read the outputs from the command.
		// this will create a pipe
    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

// test how to check if this expression is valid


    int64_t result;
    fscanf(fp, "%ld", &result);
    pclose(fp);

    printf("%ld %s\n", result, buf);
  }
  return 0;
}
