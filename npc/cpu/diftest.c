#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <dlfcn.h>
#include <assert.h>
#include "riscv.h"


// import
uint64_t* get_gpr_ptr();
void gpr_compare(uint64_t *ref);
void read_pc(uint64_t *this_pc);

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
#define gpr_num 33

static uint64_t *mem_ptr = NULL;
static bool is_sd_call = 0;
static uint64_t sd_addr = 0;


int diffverify(bool ref_exec);
int check_gpr();
int check_mem(uint64_t addr);
void diff_init(uint64_t *img, long img_size);

// .so lib name
static char ref_so_file[] = "/home/jixiang/ysyx-workbench/nemu/build/riscv64-nemu-interpreter-so";

void diff_init(uint64_t *img, long img_size){
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

void get_sd_call(uint64_t addr){
    is_sd_call = 1;
    sd_addr = addr;
}

int diffverify(bool ref_exec){
    int ret = 0;
    if(ref_exec){
        ref_difftest_exec(1);
        if(is_sd_call){
            ret = check_mem(sd_addr);
            is_sd_call = 0;
        }
    if(ret != 0) check_gpr();
    else ret = check_gpr();
    }
    return ret;
}

void diftest_getmemptr(uint64_t *p){
    mem_ptr = p;
}


int check_mem(uint64_t addr){
    int ret =0;
    uint64_t ref_mem = 0;
    ref_difftest_memcpy(addr,(void *) &ref_mem, sizeof(uint64_t),DIFFTEST_TO_DUT);
    uint64_t index = (addr-0x80000000)/8;
    if(ref_mem != mem_ptr[index]){
        printf("Mem mismatch @0x%lx, Ref=0x%lx, Dut=0x%lx\n, index=0x%lx\n",addr, ref_mem,mem_ptr[index],index);
        ret = 1;
    }
    return ret;
}

int check_gpr(){
    int i =0;
    int ret = 0;
    uint64_t this_pc = 0;
    read_pc(&this_pc);
    uint64_t *gpr = get_gpr_ptr();
    uint64_t dif_gpr[gpr_num];
    ref_difftest_regcpy(dif_gpr,DIFFTEST_TO_DUT);
    // compare
    for(i=0;i<gpr_num-1;i++){
        if(dif_gpr[i] != gpr[i]){
            printf("X[%d] mismatch! ref = %lx, dut = %lx\n",i,dif_gpr[i],gpr[i]);
            ret = -1;
        }
    }
    
    // check pc
    //if(dif_gpr[i] != this_pc){
    //    printf("X[%d] mismatch! ref = %lx, dut = %lx\n",i,dif_gpr[i],this_pc);
    //    ret = -1;
    //}

    if(ret == -1){
        gpr_compare(dif_gpr);
    }
    return ret;
}

void display_gpr(){
    uint64_t *gpr = get_gpr_ptr();
    uint64_t dif_gpr[gpr_num];
    ref_difftest_regcpy(dif_gpr,DIFFTEST_TO_DUT);
    gpr_compare(dif_gpr);
}