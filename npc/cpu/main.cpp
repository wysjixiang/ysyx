#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Vtop.h"				// contains the top class of alu module
#include <nvboard.h>
#include <stdio.h>

#define MAX_SIM_TIME 3000
#define RST_CNT 10
#define Base_ROM_Addr 0x80000000
#define NOP 0x00000013

// DIFF_TEST
#define DIFF_TEST
//#undef DIFF_TEST

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
int diffverify(bool ref_exec);
void diff_init(uint32_t *img, long img_size);
void read_inst(uint32_t *this_s);

// export
void cpu_exec(int n);
void gpr_display();
void cpu_stop();

// DPI-C: check if inst is e-break
extern "C" void dpi_that_accesses_din(svLogic din);

// 
static uint64_t sim_time =0;
static uint64_t cycle = 0;
uint32_t inst[2000] = {0};


int main(int argc, char *argv[]){
	long img_size = 0;
	img_size = ReadBinFile(argc,argv,inst);
	// difftest init
	diff_init(inst,img_size);
	// init llvm-asm lib
	init_disasm("riscv64-pc-linux-gnu");

	int sim_flag =1;
	Verilated::traceEverOn(true);
	dut -> trace(m_trace,3);
	m_trace -> open("waveform.vcd");
	reset(RST_CNT);

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
	// in/out port update when clk is low. this means it will only record what has been done!
	uint32_t inst_now = 0;
	read_inst(&inst_now);
	bool ref_exec = (inst_now != NOP);
	itrace();
	port_update();
	dut->clk = 1; 
	dut->eval(); 
	cycle++;
	m_trace->dump(sim_time++);
	#ifdef DIFF_TEST
		if(cycle > RST_CNT + 2){
			if(diffverify(ref_exec) < 0){
				printf("Difftest failed!\n");
				assert(0);
			}
		}
	#endif
}

static void first_reset(){
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


