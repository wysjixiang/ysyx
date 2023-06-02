#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Vtop.h"				// contains the top class of alu module
#include <nvboard.h>

#define MAX_SIM_TIME 3000

static TOP_NAME dut;
void nvboard_bind_all_pins(Vtop *top);
static void single_cycle();
static void reset(int n);


uint32_t inst[] ={
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00108093,
	0x00100073,  // ebreak (used as nemu_trap)

};

static uint64_t sim_time =0;

#define trace_for_wave
#ifdef trace_for_wave
	Vtop *dut_sim = new Vtop;
	VerilatedVcdC *m_trace = new VerilatedVcdC;
#endif

int main(){
    #ifdef trace_for_wave
        bool sim_flag =1;
        Verilated::traceEverOn(true);
        dut_sim -> trace(m_trace,3);
        m_trace -> open("waveform.vcd");
    #endif
	reset(10);

	int cnt =0;
    while(sim_flag){
		//nvboard_update();
		if(dut_sim->inst_addr2rom != 0){
			if(inst[cnt] != 0x00100073){
				dut_sim->inst_rom = inst[cnt++];
			} else{
				sim_flag = 0;
			}
		}
		single_cycle();
    }
	
	#ifdef trace_for_wave
		m_trace -> close();
		delete m_trace;
		delete dut_sim;
	#endif
    exit(EXIT_SUCCESS);
}

static void single_cycle() {
	dut.clk = 0; dut.eval(); 
	#ifdef trace_for_wave
		dut_sim->clk =0;
		dut_sim->eval();m_trace->dump(sim_time++);
	#endif
	dut.clk = 1; dut.eval();
	#ifdef trace_for_wave
		dut_sim->clk =1;
		dut_sim->eval();m_trace->dump(sim_time++);
	#endif
}

// reset when low
static void reset(int n) {
	#ifdef trace_for_wave
		dut_sim->rst_n = 0;
	#endif
	dut.rst_n = 0;
	while (n -- > 0) single_cycle();
	dut.rst_n = 1;
	#ifdef trace_for_wave
		dut_sim->rst_n = 1;
	#endif
}

