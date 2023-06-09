#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Vtop.h"				// contains the top class of alu module
#include <nvboard.h>
#include <stdio.h>
#include "riscv.h"

#define MAX_SIM_TIME 3000
#define RST_CNT 10
#define Base_ROM_Addr 0x80000000
#define NOP 0x00000013

#define RUNNING 1
#define QUIT 	0
#define ABORT	-1


static TOP_NAME* dut = new TOP_NAME;
static VerilatedVcdC *m_trace = new VerilatedVcdC;
void nvboard_bind_all_pins(Vtop *top);
int single_cycle();
static void reset(int n);
static void port_update();
extern "C" void init_disasm(const char *triple);

// import
int sdb_mainloop();
int parse_args(int argc, char *argv[]);
long load_img(uint32_t *mem);
int ReadBinFile(int argc, char **argv,uint32_t *mem);
void itrace();
int diffverify(int index);
void diff_init(uint64_t *img, long img_size);
void read_inst(uint32_t *this_s);
void GetMemPtr(uint64_t *p);
void diftest_getmemptr(uint64_t *p);

// export
int cpu_exec(int n);
void gpr_display();
void cpu_stop(uint64_t a0);

// DPI-C: check if inst is e-break
extern "C" void dpi_that_accesses_din(svLogic din);

// 
static uint64_t sim_time =0;
static uint64_t cycle = 0;
static int sim_flag =1;
static int cpu_state = QUIT;
uint64_t mem[STACK_SPACE] = {0};
static int ref_exec = 1;


int main(int argc, char *argv[]){

	ReadBinFile(argc,argv,(uint32_t *)mem);
	// difftest init
	diff_init(mem,8*STACK_SPACE);
	// init llvm-asm lib
	init_disasm("riscv64-pc-linux-gnu");
	// get mem_ptr for dpi
	GetMemPtr(mem);
	diftest_getmemptr(mem);

	//Verilated::traceEverOn(true);
	//dut -> trace(m_trace,3);
	reset(RST_CNT);

    while(sim_flag){
		if(-1 == sdb_mainloop()){
			break;
		}
    }

	delete m_trace;
	delete dut;
    //exit(EXIT_SUCCESS);
	if(cpu_state == QUIT){
		printf("\033[32mHIT GOOD TRAP!\033[0m\n");
	} else{
		printf("\033[31mHIT BAD TRAP!\033[0m\n");
	}
	return cpu_state;
}

static void port_update(){
	uint64_t addr = (dut->inst_addr2rom - Base_ROM_Addr) /4;
	uint32_t *inst_ptr = (uint32_t *)mem;
	dut->inst_rom = inst_ptr[addr];
}

int single_cycle() {
	int ret =0;
	ref_exec = REF_RUN;
	dut->clk = 0; dut->eval();
	// in/out port update when clk is low. this means it will only record what has been done!
	uint32_t inst_now = 0;
	read_inst(&inst_now);
	#ifdef I_TRACE
		itrace();
	#endif
	port_update();
	dut->clk = 1; 
	dut->eval(); 
	if(ref_exec == REF_RUN){
		ref_exec = (inst_now != NOP);
	}
	cycle++;
	#ifdef DIFF_TEST
		if(cycle > RST_CNT + 2){
			if(diffverify(ref_exec) < 0){
				printf("Difftest failed!\n");
				//assert(0);
				ret = 1;
			}
		}
	#endif
	return ret;
}

static void first_reset(){
	dut->rst_n = 0;
	dut->clk = 0; dut->eval(); 
	dut->clk = 1; dut->eval(); 
}
// reset when low
static void reset(int n) {
	first_reset();
	dut->rst_n = 0;
	while (n -- > 0) single_cycle();
	dut->rst_n = 1;
}

void cpu_stop(uint64_t a0){
	sim_flag = 0;
	if(a0 == 0){
		cpu_state = QUIT;
	} else{
		cpu_state = ABORT;
	}
}

int cpu_exec(int n){
	int ret =0;
	while(n--){
		if(sim_flag == 0) return -1;
		ret = single_cycle();
		if(ret != 0) break;
	}
	return ret;
}

void ref_skip(int index){
	ref_exec = index;
}
