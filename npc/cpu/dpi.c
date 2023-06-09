#include "svdpi.h"
#include "verilated_dpi.h"

#include <time.h>
#include <stdio.h>

#include "riscv.h"


// var
static uint64_t *mem_ptr = NULL;
static uint32_t pc = 0;
static uint32_t inst = 0;
uint64_t *cpu_gpr = NULL;

static struct {
	const char *name;
	uint64_t addr;
	int space;
}	MMIO_MAP[] = {
	{"UART",	UART_ADDR,8},
	{"RTC",		RTC_ADDR,8},
};

#define ARRLEN(arr) (int)(sizeof(arr) / sizeof(arr[0]))
#define NR_DEV ARRLEN(MMIO_MAP)

// import
void cpu_stop(uint64_t a0);
void itrace(uint64_t pc, uint8_t *s);
void ref_skip(int index);

//export
void GetMemPtr(uint64_t *p){
	mem_ptr = p;
}

void get_sd_call(uint64_t addr);
int check_mem(uint64_t addr);

// DPI-C: check if inst is e-break
extern "C" void dpi_that_accesses_din(svLogic din);

// DPI-C: read/ write mem
extern "C" void pmem_read(long long raddr, long long *rdata, unsigned char ena);
extern "C" void pmem_write(long long waddr, long long wdata, unsigned char wmask,unsigned char ena);



extern "C" void dpi_that_accesses_din(svLogic din) {
	if(din){
        cpu_stop(cpu_gpr[10]); // return a0 for good / bad hit check
	}
	return ;
}


// DPI-C: read gpr
extern "C" void set_gpr_ptr(const svOpenArrayHandle r) {
  cpu_gpr = (uint64_t *)(((VerilatedDpiOpenVar*)r)->datap());
}

#define gpr_num 33
const char *regs_name[] = {
  "x0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
  "pc"
};


// 一个输出RTL中通用寄存器的值的示例
void gpr_display() {
  int i =0;
  for (i = 0; i < gpr_num-1; i++) {
    printf("X[%2d](%3s) = 0x%lx\n", i,regs_name[i], cpu_gpr[i]);
  }
	printf("X[%2d](%3s) = 0x%x\n", i,regs_name[i], pc);
}

void gpr_compare(uint64_t *ref){
	int i=0;
	for(i=0;i<gpr_num-1;i++){
		printf("X[%2d](%3s): Ref=0x%16lx , Dut=0x%16lx\n", i,regs_name[i],ref[i] , cpu_gpr[i]);
	}
	//compare pc
	printf("X[%2d](%3s): Ref=0x%16lx , Dut=0x%8x\n", i,regs_name[i],ref[i] , pc);
}

uint64_t* get_gpr_ptr(){
	return cpu_gpr;
}

// DPI-C: read mem


void mem_display(int index){
	printf("Mem Value = %lx @addr:%x\n",mem_ptr[index],index * 8 + BASE_ADDR);
}


void read_pc(uint64_t *this_pc){
	*this_pc = pc;
}

void read_inst(uint32_t *this_s){
	*this_s = inst;
}

extern "C" void get_pc (svLogicVecVal* a){
	pc = a->aval;
}

extern "C" void get_inst (svLogicVecVal* a){
	inst = a->aval;
}


long long vaddr2phy(long long x){
	if( x < BASE_ADDR || x >= BASE_ADDR + STACK_SPACE){
		printf("Out of band! illegal addr:%llx\n",x);
		assert(0);
	}
	return (x - BASE_ADDR)/8;
}

static uint64_t bool_time = 0;
static uint64_t now_time = 0;

static uint64_t get_time_internal() {
  struct timespec now;
  clock_gettime(CLOCK_MONOTONIC_COARSE, &now);
  uint64_t us = now.tv_sec * 1000000 + now.tv_nsec / 1000;
  return us;
}

void get_time() {
  if (bool_time == 0) bool_time = get_time_internal();
  now_time = get_time_internal() - bool_time;
}

int mmio_addr_check(uint64_t addr){
	for(int i=0;i<NR_DEV;i++){
		// addr range 8 bytes
		if(addr >= MMIO_MAP[i].addr  && (addr < MMIO_MAP[i].addr + 8*MMIO_MAP[i].space)){
			return i;
		}
	}
	return -1;
}

int mmio_read(uint64_t addr, void* buf ,int len){
	int index = mmio_addr_check(addr);
	if(index == -1){
		return -1;
	}
	return 0;
}

void mmio_write(uint64_t addr, uint64_t data, uint8_t mask){

}




extern "C" void pmem_read(long long raddr, long long *rdata,unsigned char ena) {
	//printf("raddr = %llx, ena = %d\n",raddr,ena);

  // 总是读取地址为`raddr & ~0x7ull`的8字节返回给`rdata`
  if(ena == 1){

	int index = mmio_addr_check(raddr);
	if(index != -1){
		ref_skip(REF_SKIP_READ);
		if(MMIO_MAP[index].name == "RTC"){
			if(raddr == RTC_ADDR){
				*rdata = now_time;
			} else{
				get_time();
				*rdata = now_time >> 32;
			}
			return ;
		}
	}
	uint64_t addr_align = raddr & ~0x7ull;
	*rdata = mem_ptr[vaddr2phy(addr_align)];
	#ifdef DIFF_TEST
		check_mem(addr_align);
	#endif
  }
}
extern "C" void pmem_write(long long waddr, long long wdata, unsigned char wmask,unsigned char ena) {
  // 总是往地址为`waddr & ~0x7ull`的8字节按写掩码`wmask`写入`wdata`
  // `wmask`中每比特表示`wdata`中1个字节的掩码,
  // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
	//printf("waddr = %llx, wdata = %llx, mask = %d, ena = %d\n",waddr, wdata , wmask, ena);
	
	if(ena == 1){

		// uart
		int index = mmio_addr_check(waddr);
		if(index != -1){
			ref_skip(REF_SKIP_WRITE);
			if(MMIO_MAP[index].name == "UART"){
				switch(wmask){
					case 0xff:
						for(int i=0;i<4;i++){
							putchar((uint8_t)wdata);
							wdata >> 8;
						}
						break;
					case 0xf:
						for(int i=0;i<2;i++){
							putchar((uint8_t)wdata);
							wdata >> 8;
						}
						break;
					case 0x3:
						for(int i=0;i<1;i++){
							putchar((uint8_t)wdata);
							wdata >> 8;
						}
						break;
					case 0x1:
						putchar((uint8_t)wdata);
						break;
				}
			}
			return ;
		}

		uint64_t addr_align = waddr & ~0x7ull;
		int addr_dif = waddr - addr_align;
		unsigned char byte_data = 0;
		long long mem_data = mem_ptr[vaddr2phy(addr_align)];
		long long new_data = wdata;
		
		uint8_t *p = (uint8_t *)mem_ptr ;
		int step = waddr - BASE_ADDR;
		switch(wmask){
			case 0xff:
				mem_ptr[vaddr2phy(addr_align)] = new_data;
				break;
			case 0xf:
				memcpy(p+step,&new_data,4);
				break;
			case 0x3:
				memcpy(p+step,&new_data,2);
				break;
			case 0x1:
				memcpy(p+step,&new_data,1);
				break;
		}

		//printf("new_data = %llx\n",new_data);
		//printf("mem_data = %lx\n",mem_ptr[vaddr2phy(addr_align)]);
		get_sd_call(addr_align);
	}
}