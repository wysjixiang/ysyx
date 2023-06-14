#include <stdint.h>
#include <stdio.h>
#include <NDL.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif


int main() {
	
	uint32_t time_now =0;
	uint32_t time_last =0;

	while(1){
		time_now = NDL_GetTicks();

		if(time_now - time_last > 500){
			time_last = time_now;
			printf("time last = %d\n",time_last);
			printf("0.5s passed!\n");
		}
	}
  return 0;
}
