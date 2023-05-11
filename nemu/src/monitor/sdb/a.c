#include <stdio.h>
#include <regex.h>
#include <assert.h>

int main(){
	regex_t regex;
	int ret;

	char *expression = "(a $a1 - (0x333+1)2 / (2+1))";
	
	char *rule = "\\$[a-zA-z][0-9]";


	ret = regcomp(&regex,rule,REG_EXTENDED);
	if(ret) assert(0);

	regmatch_t match;
	ret = regexec(&regex,expression,1,&match,0);
	if(ret == REG_NOMATCH){
		assert(0);
	}	else if(ret == 0){
		printf("matched\n");
		for (int i = match.rm_so; i < match.rm_eo; i++) {
			printf("%c", expression[i]);
		}	
			printf("\n");
	}	else assert(0);
	return 0;
}
