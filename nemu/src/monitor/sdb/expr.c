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

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
// this is a library function for regular expression
//  regcomp()  is  used  to compile a regular expression into a form 
//	that is suitable for subsequent regexec() searches.
//
//
#include <memory/paddr.h>



enum {
  TK_NOTYPE = 256,
	TK_EQ,
	TK_NEQ,
	TK_LAND,
	TK_LBRACKET,
	TK_RBRACKET,
	TK_HEX,
	TK_REG,
	TK_NUM,

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
//  Attention! this is regular expression, so take care of escape character
  {" +", TK_NOTYPE},    // spaces   ' +'means at least one blankspace
//  !why need two \\? since \+ means + in regular expression. but 
//	we use "", so we need to escape'\' so we use "\\+" to represents +
	{"\\+", '+'},					 // plus      
  {"-",		'-'},					 // minus
  {"\\*",		'*'},        // mult
  {"\\/",		'/'},        // div
  {"==", TK_EQ},			 	// equal
  {"!=", TK_NEQ},        // not equal
  {"&&", TK_LAND},        // logic and
  {"\\(",TK_LBRACKET},    // left bracket
  {"\\)",TK_RBRACKET},    // right bracket
  {"0x[0-9a-fA-F]+",TK_HEX},        // hex number
  {"\\$[a-zA-Z0-9]+",TK_REG},   // register 
  {"[0-9]+",TK_NUM},        // number

  
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

// write rules to re[] array for later use
  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
	word_t value;
} Token;


#define TOKENNUM 10000


static Token tokens[TOKENNUM] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        //char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //   i, rules[i].regex, position, substr_len, substr_len, substr_start);

        

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
			static int j= 0;
			word_t result = 0;

				switch (rules[i].token_type) {
					case	TK_NOTYPE:
						break;
					case	TK_HEX:
						tokens[nr_token].type = rules[i].token_type;
						j= 0;
						while(j < substr_len){
							tokens[nr_token].str[j] = e[position + j];
							j++;
						}
						tokens[nr_token].str[j] = '\0';
						result = 0;
						j = 2;
						while(j < substr_len){
							if(e[position + j] >= '0' && e[position + j] <= '9'){
								result = result * 16 + e[position + j] - '0';
							}	else if(e[position + j] >= 'A' && e[position + j] <= 'F'){
								result = result * 16 + e[position + j] - 'A' + 10 ;
							}	else{
								result = result * 16 + e[position + j] - 'a' + 10;
							}
							j++;
						}
						tokens[nr_token].value = result;
						nr_token++;

						break;
					case	TK_REG:
						tokens[nr_token].type = rules[i].token_type;
						j= 0;
						while(j < substr_len){
							tokens[nr_token].str[j] = e[position + j+1];
							j++;
						}
						tokens[nr_token].str[j] = '\0';
						bool str2val_success = 0;
						tokens[nr_token].value = isa_reg_str2val(tokens[nr_token].str,&str2val_success);
						if(str2val_success == 0){
							printf("Error when reading value from gpr\n");
							assert(0);
						}
						nr_token++;
						break;
					case	TK_NUM:
						tokens[nr_token].type = rules[i].token_type;
						j= 0;
						while(j < substr_len){
							tokens[nr_token].str[j] = e[position + j];
							j++;
						}
						tokens[nr_token].str[j] = '\0';
						result = 0;
						j = 0;
						while(j < substr_len){
							result = result * 10 + e[position + j] - '0';
							j++;
						}
						tokens[nr_token].value = result;
						nr_token++;

						break;
          default:		
						tokens[nr_token].type = rules[i].token_type;
						nr_token++;
						break;
        }
				position += substr_len;
				break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  return true;
}



// operator
enum {
	
	op_num = 0,
	op_add,
	op_minus,
	op_div,
	op_lb,
  op_rb,
  op_eq,
  op_neq,
  op_land,
  op_mult_deref,
};

enum {
	
	prio_mult_div = 2,
	prio_add_minus = 3,
	prio_shift = 4,

	prio_eq_nqe =6,
	prio_land = 7,


};



static void FindMainOp(int p_start, int p_end, int* args){
	int p = p_start;
	int q = p_end;

	if(!(tokens[q].type == TK_RBRACKET || tokens[q].type == TK_NUM || tokens[q].type == TK_HEX || tokens[q].type == TK_REG )){
		printf("The end of expression is invalid\n");
		assert(0);
	}

	int num =0;	// number of difference of ( and )
	int pre_op = -1;	
	// save previous sign to handle if errors and if mainop
	
	// pos to record if it is a binocular operator!
	// 1 if binocular, 0 if non
	int index = q-p+1;

	// priority 
	int prio = -1;
	int prio_pos = 0;


// while loop for token recognization and unary operator
	while( p <= q){
		if(num > 0 || tokens[p].type == TK_LBRACKET || tokens[p].type == TK_RBRACKET){

			pre_op = op_rb;	// if it is in a ( ), there must not be a mainop, so we just search after )
			if(tokens[p].type == TK_LBRACKET){
				num++;
			}	else if(tokens[p].type == TK_RBRACKET){
				num--;
			}
		}	else{
			switch(tokens[p].type){
				case '+':
					if(pre_op == op_num || pre_op == op_rb){

						if(prio <= prio_add_minus) {
							prio = prio_add_minus;
							prio_pos = p;
						}
					}	
					pre_op = op_add;
					break;
				
				case '-':
					if(pre_op == op_num || pre_op == op_rb){

						if(prio <= prio_add_minus) {
							prio = prio_add_minus;
							prio_pos = p;
						}
					}
					pre_op = op_minus;
					break;

				case '*':
					if(pre_op == op_num || pre_op == op_rb){

						if(prio <= prio_mult_div) {
							prio = prio_mult_div;
							prio_pos = p;
						}
					}	
					pre_op = op_mult_deref;
					break;

				case '/':
					if(pre_op == op_num || pre_op == op_rb){

						if(prio <= prio_mult_div) {
							prio = prio_mult_div;
							prio_pos = p;
						}
					}	else{
						printf("Expression Error! invalid sign before /\n");
						assert(0);
					}
					pre_op = op_div;
					break;

				case TK_LAND:
					if(!(pre_op == op_num || pre_op == op_rb)){
						printf("Expression Error! invalid sign before &&\n");
						assert(0);
					}
					if(prio <= prio_land) {
						prio = prio_land;
						prio_pos = p;	
					}

					pre_op = op_land;
					break;

				case TK_EQ:
					if(!(pre_op == op_num || pre_op == op_rb)){
						printf("Expression Error! invalid sign before ==\n");
						assert(0);
					}
					if(prio <= prio_eq_nqe) {
						prio = prio_eq_nqe;
						prio_pos = p;
					}

					pre_op = op_eq;
					break;
					
				case TK_NEQ:
					if(!(pre_op == op_num || pre_op == op_rb)){
						printf("Expression Error! invalid sign before !=\n");
						assert(0);
					}
					if(prio <= prio_eq_nqe) {
						prio = prio_eq_nqe;
						prio_pos = p;
					}

					pre_op = op_neq;
					break;

				case TK_HEX:

					pre_op = op_num;
					break;

				case TK_REG:

					pre_op = op_num;
					break;

				case TK_NUM:
					pre_op = op_num;
					break;
				
				default:
					printf("Unknown character in expression\n");
					assert(0);
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
	if(num != 0 ){
		printf("The number of ( and ) is not paired\n");
		assert(0);
	}

// if prio == -1, means it is a numebr

	int j= 0;	
	bool sign_flag = 1;
	bool flag_deref = 0;

	if(prio == -1){
		// no MainOp
		// first check if it is a dereference
		for(j=0;j<index;j++){
			if(tokens[j+p_start].type == '*') {
			 	flag_deref = 1;
				break;
			}
		}

		if(flag_deref){
			args[0] = 1;
			args[1] = j + p_start ;
			args[2] = 1;
			args[3] = sign_flag;
			return ;
		}	else{
			// if it is not a dereference, make sure no * character.
			sign_flag = 1;
			for(int i=0; i<index;i++){
				if(tokens[p_start+i].type == TK_NUM ||tokens[p_start+i].type == TK_REG || tokens[p_start+i].type == TK_HEX ){
					printf("Invalid EXPR. There is no operation sign!\n");
					assert(0);
				}
				if(tokens[p_start+i].type == '-') sign_flag = !sign_flag;
			}
			args[0] = 0;
			args[1] = p_end - 1;
			args[2] = 0;
			args[3] = sign_flag;
			return;

		}
	}	else{
		// there is to be MainOp
		// we have find the MainOp already when we analyze the expression.
		// the order is prio_pos
		
			args[0] = 1;
			args[1] = prio_pos;
			args[2] = 0;
			args[3] = 0;
			return;

	}

	// check if there is to be a MainOp. if no MainOp, this expression should be a number with prefix of + or - or combination of them
	// this is a special case like ++++----+---++++ number
	// obviously, the number is at the end of expression or this expression would be considered as an invalid one
	// so the number's address is the pos[2][index-1]+1, the sign should be calculate via unary operators
	
	#undef	max_len
}


static bool check_parentheses(int p,int q){
	if(tokens[p].type == TK_LBRACKET && tokens[q].type == TK_RBRACKET){
		int num = 1;
		p++;
		bool flag_not_paired = 0;
		while(p != q){
			if(tokens[p].type == TK_LBRACKET){
				num++;
			}	else if(tokens[p].type == TK_RBRACKET){
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
	}	else{
		return false;
	}
}



word_t eval(int p, int q){
	if(p > q){
		//bad expression
		printf("Invalid Expression\n");
		assert(0);
	} else if(p == q){
		if(tokens[p].type == TK_NUM || tokens[p].type == TK_HEX || tokens[p].type == TK_REG){
			return tokens[p].value;
		}	else{
			printf("Invalid non-number value\n");
			assert(0);
		}
	}	else if(check_parentheses(p,q) == true){
		return eval(p+1,q-1);
	}	else{
			int args[4];
			/*
			args[0] -> 0 means number; 1 means op;
			args[1] -> address
			args[2] -> 1 means dereference
			args[3] -> 1 positive, 0 negetive
			*/
			//find main op
			FindMainOp(p,q,args);
			// printf("offset = %d\n",args[0]);
			// printf("opcode = %d\n",args[1]);
			// printf("sign = %d\n",args[2]);
			int addr = args[1];
			int sign = args[3] ? 1 : -1;
	
			if(args[0]){
				switch(tokens[addr].type){
					case '+':
						return eval(p,addr-1) + eval(addr+1,q);
						break;

					case '-':
						return eval(p,addr-1) - eval(addr+1,q);
						break;

					case '*':
					// if it is a dereference command
						if(args[2] == 1){
							return sign * paddr_read((eval(addr+1,q)),8);
						}	else{
							return eval(p,addr-1) * eval(addr+1,q);
						}
						break;

					case '/':
						word_t dividend = eval(addr+1,q);
						if(dividend == 0) {
							printf("Can not div 0\n");
							assert(0);
						}	else{
							return eval(p,addr-1) / dividend;
						}
						break;

					case TK_EQ:
						return eval(p,addr-1) ==  eval(addr+1,q);
						break;

					case TK_NEQ:
						return eval(p,addr-1) != eval(addr+1,q);
						break;


					case TK_LAND:
						return eval(p,addr-1) && eval(addr+1,q);
						break;

					default:
						assert(0);


				}
			}	else{
				// if it is a number
				return (sign * tokens[addr+1].value);

			}
	}
}


word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  /* TODO: Insert codes to evaluate the expression. */

	*success = 1;
	return eval(0,nr_token-1);

}
