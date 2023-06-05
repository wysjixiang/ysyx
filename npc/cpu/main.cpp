#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Vtop.h"				// contains the top class of alu module
#include <nvboard.h>
#include <stdio.h>

#define MAX_SIM_TIME 3000
#define Base_ROM_Addr 0x80000000

static TOP_NAME* dut = new TOP_NAME;
static VerilatedVcdC *m_trace = new VerilatedVcdC;
void nvboard_bind_all_pins(Vtop *top);
static void single_cycle();
static void reset(int n);
static void port_update();
extern "C" void init_disasm(const char *triple);

// import
int sdb_mainloop();
int parse_args(int argc, char *argv[]);
long load_img(uint32_t *inst);
int ReadBinFile(int argc, char **argv,uint32_t *inst);
void itrace();

// export
void cpu_exec(int n);
void gpr_display();
void cpu_stop();

// DPI-C: check if inst is e-break
extern "C" void dpi_that_accesses_din(svLogic din);

// 
static uint64_t sim_time =0;
uint32_t inst[2000] = {0};


int main(int argc, char *argv[]){

	ReadBinFile(argc,argv,inst);
	// init llvm-asm lib
	init_disasm("riscv64-pc-linux-gnu");

	int sim_flag =1;
	Verilated::traceEverOn(true);
	dut -> trace(m_trace,3);
	m_trace -> open("waveform.vcd");
	reset(10);

	int cnt =0;
    while(sim_flag != -1){
		sim_flag = sdb_mainloop();
    }
	
	m_trace -> close();
	delete m_trace;
	delete dut;
    exit(EXIT_SUCCESS);
}

static void port_update(){
	uint64_t addr = (dut->inst_addr2rom - Base_ROM_Addr) /4;
	dut->inst_rom = inst[addr];
}

static void single_cycle() {
	dut->clk = 0; dut->eval(); m_trace->dump(sim_time++);
	dut->clk = 1; 
	// in/out port update when clk is high, but before update
	itrace();
	port_update();
	dut->eval(); 
	m_trace->dump(sim_time++);
}

void first_reset(){
	dut->rst_n = 0;
	dut->clk = 0; dut->eval(); m_trace->dump(sim_time++);
	dut->clk = 1; dut->eval(); m_trace->dump(sim_time++);
}
// reset when low
static void reset(int n) {
	first_reset();
	dut->rst_n = 0;
	while (n -- > 0) single_cycle();
	dut->rst_n = 1;
}

void cpu_stop(){
	m_trace -> close();
	delete m_trace;
	delete dut;
	exit(EXIT_SUCCESS);
}

void cpu_exec(int n){
	while(n--){
		single_cycle();
	}
}

