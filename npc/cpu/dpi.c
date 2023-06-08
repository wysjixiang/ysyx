#include "svdpi.h"
#include "verilated_dpi.h"


// define
#define BASE_ADDR 0x80000000
#define STACK_SPACE 0x9000


// var
static uint64_t *mem_ptr = NULL;
static uint32_t pc = 0;
static uint32_t inst = 0;




// import
void cpu_stop();
void itrace(uint64_t pc, uint8_t *s);
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
        cpu_stop();
	}
	return ;

}


// DPI-C: read gpr
uint64_t *cpu_gpr = NULL;
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
	if( x < BASE_ADDR || x > BASE_ADDR + STACK_SPACE){
		printf("Out of band! illegal addr:%llx\n",x);
		assert(0);
	}
	return (x - BASE_ADDR)/8;
}

extern "C" void pmem_read(long long raddr, long long *rdata,unsigned char ena) {
	printf("raddr = %llx, ena = %d\n",raddr,ena);
  // 总是读取地址为`raddr & ~0x7ull`的8字节返回给`rdata`
  if(ena == 1){
	uint64_t addr_align = raddr & ~0x7ull;
	*rdata = mem_ptr[vaddr2phy(addr_align)];
	check_mem(addr_align);
  }
}
extern "C" void pmem_write(long long waddr, long long wdata, unsigned char wmask,unsigned char ena) {
  // 总是往地址为`waddr & ~0x7ull`的8字节按写掩码`wmask`写入`wdata`
  // `wmask`中每比特表示`wdata`中1个字节的掩码,
  // 如`wmask = 0x3`代表只写入最低2个字节, 内存中的其它字节保持不变
	printf("waddr = %llx, wdata = %llx, mask = %d, ena = %d\n",waddr, wdata , wmask, ena);
	if(ena == 1){
		uint64_t addr_align = waddr & ~0x7ull;
		int addr_dif = waddr - addr_align;
		unsigned char byte_data = 0;
		long long mem_data = mem_ptr[vaddr2phy(addr_align)];
		long long new_data = wdata;
		//for(int i=0;i<8;i++){
		//	byte_data = 0;
		//	if((wmask >> 8 -i -1) & 0x01 ){
		//		byte_data = ((wdata >> 8*(8-i-1)) & 0xFF );
		//	} else {
		//		byte_data = ((mem_data >> 8*(8-i-1)) & 0xFF );
		//	}
		//	new_data = (new_data << 8) | byte_data;
		//}
		
		uint8_t *p = (uint8_t *)mem_ptr ;
		int index = waddr - BASE_ADDR;
		switch(wmask){
			case 0xff:
				mem_ptr[vaddr2phy(addr_align)] = new_data;
				break;
			case 0xf:
				memcpy(p+index,&new_data,4);
				break;
			case 0x3:
				memcpy(p+index,&new_data,2);
				break;
			case 0x1:
				memcpy(p+index,&new_data,1);
				break;
		}

		
		printf("new_data = %llx\n",new_data);
		printf("mem_data = %lx\n",mem_ptr[vaddr2phy(addr_align)]);
		get_sd_call(addr_align);
	}
}