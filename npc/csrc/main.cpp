#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Vtop.h"				// contains the top class of alu module
#include <nvboard.h>

#define MAX_SIM_TIME 30



static TOP_NAME dut;
void nvboard_bind_all_pins(Vtop *top);


int main(){
		Vtop *dut_sim = new Vtop;
    nvboard_bind_all_pins(&dut);
    nvboard_init();
    Verilated::traceEverOn(true);
		VerilatedVcdC *m_trace = new VerilatedVcdC;
		dut_sim -> trace(m_trace,5);
		m_trace -> open("waveform.vcd");

		uint64_t sim_time = 0;
		while(sim_time < MAX_SIM_TIME){
			dut_sim -> sw = rand()%4;
			dut_sim->eval();
			m_trace -> dump(sim_time);
			sim_time++;
		}
		m_trace -> close();
		delete dut_sim;

    while(1){
         nvboard_update();
         dut.eval();
    }
    exit(EXIT_SUCCESS);
}



