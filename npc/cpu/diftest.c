#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// `direction`为`DIFFTEST_TO_DUT`时, 获取REF的寄存器状态到`dut`;
// `direction`为`DIFFTEST_TO_REF`时, 设置REF的寄存器状态为`dut`;
#define DIFFTEST_TO_DUT 0
#define DIFFTEST_TO_REF 1

// import
uint64_t* get_gpr_ptr();


// dynamic lib func
extern void (*difftest_memcpy)(uint64_t addr, void *buf, size_t n, bool direction);
extern void (*difftest_regcpy)(void *dut, bool direction);
extern void (*difftest_exec)(uint64_t n);
extern void (*difftest_raise_intr)(uint64_t NO);
extern void (*difftest_init)(int port);



int diffverify();
int check_gpr();
int check_mem();
int diff_init();



int diff_init(char *ref_so_file, long img_size){
    // first get the function ptr from so lib




    // init
    difftest_init(0);
    // copy bin file to guest ram



}



int diffverify(){
    int ret = 0;
    check_mem();
    ret = check_gpr();

    return ret;
}

int check_mem(){
    return 0;
}

int check_gpr(){
    int ret = 0;
    uint64_t *gpr = get_gpr_ptr();
    uint64_t dif_gpr[32];
    difftest_regcpy(dif_gpr,DIFFTEST_TO_DUT);
    // compare
    for(int i=0;i<32;i++){
        if(dif_gpr[i] != gpr[i]){
            printf("X[%d] mismatch! ref = %lx, dut = %lx\n",i,dif_gpr[i],gpr[i]);
            ret = -1;
        }
    }
    return ret;
}