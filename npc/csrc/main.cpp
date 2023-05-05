#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Vtop.h"				// contains the top class of alu module
#include <nvboard.h>

#define MAX_SIM_TIME 3000

static TOP_NAME dut;
void nvboard_bind_all_pins(Vtop *top);

int main(){
		Vtop *dut_sim = new Vtop;
    nvboard_bind_all_pins(&dut);
    nvboard_init();
    Verilated::traceEverOn(true);
		VerilatedVcdC *m_trace = new VerilatedVcdC;
		dut_sim -> trace(m_trace,2);
		m_trace -> open("waveform.vcd");


		// sub verification
		uint64_t sim_time = 0;
		bool flag = 1;
		int16_t a =0;
		int16_t b = 0;
		uint8_t op = 1;
		while(flag){
			op = 1;
			//dut_sim -> sw = rand()%4;
			for(a = -8; a<8; a++){
				for(b=-8;b<8;b++){
					dut_sim->sw = ((a<<4 | (b & 0xF)) &0xFF ) | (op << 13);
					dut_sim->eval();
					m_trace -> dump(sim_time);
					sim_time++;
					if(a == 7 && b == 7) flag = 0;
				}
			}
		}

		// blt verification
		flag = 1;
		op = 7;
		while(flag){
			//dut_sim -> sw = rand()%4;
			for(a = -8; a<8; a++){
				for(b=-8;b<8;b++){
					dut_sim->sw = ((a<<4 | (b & 0xF)) &0xFF ) | (op << 13);
					dut_sim->eval();
					m_trace -> dump(sim_time);
					sim_time++;
					if(a == 7 && b == 7) flag = 0;
				}
			}
		}
		m_trace -> close();
		delete dut_sim;

    while(1){
         nvboard_update();
         dut.eval();
    }
    exit(EXIT_SUCCESS);
}



