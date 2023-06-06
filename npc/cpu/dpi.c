#include "svdpi.h"
#include "verilated_dpi.h"

// DPI-C: check if inst is e-break
extern "C" void dpi_that_accesses_din(svLogic din);

// import
void cpu_stop();
void itrace(uint64_t pc, uint8_t *s);


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

// 一个输出RTL中通用寄存器的值的示例
void gpr_display() {
  int i;
  for (i = 0; i < 32; i++) {
    printf("X[%d] = 0x%lx\n", i, cpu_gpr[i]);
  }
}

uint64_t* get_gpr_ptr(){
	return cpu_gpr;
}

// DPI-C: read mem


void mem_display(){
	;
}

static uint32_t pc = 0;
static uint32_t inst = 0;



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