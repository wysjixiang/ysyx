#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <assert.h>

// `direction`为`DIFFTEST_TO_DUT`时, 获取REF的寄存器状态到`dut`;
// `direction`为`DIFFTEST_TO_REF`时, 设置REF的寄存器状态为`dut`;
#define DIFFTEST_TO_DUT 0
#define DIFFTEST_TO_REF 1
#define BASE_ADDR 0x80000000

// import
uint64_t* get_gpr_ptr();

// dynamic lib func
void (*ref_difftest_memcpy)(uint64_t addr, void *buf, size_t n, bool direction) = NULL;
void (*ref_difftest_regcpy)(void *dut, bool direction) = NULL;
void (*ref_difftest_exec)(uint64_t n) = NULL;
void (*ref_difftest_raise_intr)(uint64_t NO) = NULL;
void (*ref_difftest_init)(int port) = NULL;

/* The MODE argument to `dlopen' contains one of the following: */
#define RTLD_LAZY	0x00001	/* Lazy function call binding.  */
#define RTLD_NOW	0x00002	/* Immediate function call binding.  */
#define	RTLD_BINDING_MASK   0x3	/* Mask of binding time value.  */
#define RTLD_NOLOAD	0x00004	/* Do not load the object.  */
#define RTLD_DEEPBIND	0x00008	/* Use deep binding.  */



int diffverify(bool ref_exec);
int check_gpr();
int check_mem();
void diff_init(uint32_t *img, long img_size);

// .so lib name
static char ref_so_file[] = "/home/jixiang/ysyx-workbench/nemu/build/riscv64-nemu-interpreter-so";

void diff_init(uint32_t *img, long img_size){
    // first get the function ptr from so lib
    void *handle;
    handle = dlopen(ref_so_file, RTLD_LAZY);
    assert(handle);
    ref_difftest_memcpy = (void(*)(uint64_t, void *, size_t, bool))dlsym(handle, "difftest_memcpy");
    assert(ref_difftest_memcpy);
    ref_difftest_regcpy = (void(*)(void *, bool))dlsym(handle, "difftest_regcpy");
    assert(ref_difftest_regcpy);
    ref_difftest_exec = (void(*)(uint64_t))dlsym(handle, "difftest_exec");
    assert(ref_difftest_exec);
    ref_difftest_raise_intr = (void(*)(uint64_t))dlsym(handle, "difftest_raise_intr");
    assert(ref_difftest_raise_intr);
    void (*ref_difftest_init)(int) = (void (*)(int))dlsym(handle, "difftest_init");
    assert(ref_difftest_init);

    // init 
    ref_difftest_init(0);
    // copy bin file to ref ram
    ref_difftest_memcpy(BASE_ADDR,(void *) img,img_size,DIFFTEST_TO_REF);
}



int diffverify(bool ref_exec){
    int ret = 0;
    if(ref_exec){
        ref_difftest_exec(1);
        check_mem();
        ret = check_gpr();
    }
    return ret;
}

int check_mem(){
    return 0;
}

int check_gpr(){
    int ret = 0;
    uint64_t *gpr = get_gpr_ptr();
    uint64_t dif_gpr[32];
    ref_difftest_regcpy(dif_gpr,DIFFTEST_TO_DUT);
    // compare
    for(int i=0;i<32;i++){
        if(dif_gpr[i] != gpr[i]){
            printf("X[%d] mismatch! ref = %lx, dut = %lx\n",i,dif_gpr[i],gpr[i]);
            ret = -1;
        }
    }
    return ret;
}