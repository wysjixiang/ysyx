#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Valu.h"				// contains the top class of alu module

#define MAX_SIM_TIME 20

uint64_t sim_time = 0;
uint64_t posedge_cnt = 0;

// sync rst, valid when assert 1
//void dut_reset(Valu *dut, uint64_t &sim_time){
//	dut->rst =0;
//	if(sim_time < 5)
//	{
//		dut -> rst = 1;
//		dut -> a = 0;
//		dut -> b = 0;
//	}
//}


int main(int argc,char* para){

	Valu *dut = new Valu;
	Verilated::traceEverOn(true);
	VerilatedVcdC *m_trace = new VerilatedVcdC;
	dut -> trace(m_trace,5);
	m_trace -> open("waveform.vcd");

	while(sim_time < MAX_SIM_TIME){
		
		dut -> a = rand() % 2;
		dut -> b = rand() % 2;
		dut -> eval();
		std::cout<<"a ^ b = "<< dut -> f << std::endl;
			
		m_trace->dump(sim_time);
		sim_time++;

	}
	
	m_trace->close();
	delete dut;
	exit(EXIT_SUCCESS);
}
