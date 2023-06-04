#include <iostream>
#include <verilated.h>	//common verilator routiness
#include <verilated_vcd_c.h>	// write waveforms to a VCD file
#include "Vtop.h"				// contains the top class of alu module
#include <nvboard.h>
#include "svdpi.h"

#define MAX_SIM_TIME 3000

static TOP_NAME* dut = new TOP_NAME;
static VerilatedVcdC *m_trace = new VerilatedVcdC;
void nvboard_bind_all_pins(Vtop *top);
static void single_cycle();
static void reset(int n);
static void port_update();

// DPI-C: check if inst is e-break
extern "C" void dpi_that_accesses_din(svLogic din);

uint32_t inst[] ={
	0x00000413,
	0x00009117,
	0xffc10113,
	0x00c000ef,
	0x00000513,
	0x00008067,
	0xff010113,
	0x00000517,
	0x01c50513,
	0x00113423,
	0xfe9ff0ef,
	0x00050513,
	0x00100073,
	0x0000006f
};

static uint64_t sim_time =0;
#define Base_ROM_Addr 0x80000000


int main(){
	bool sim_flag =1;
	Verilated::traceEverOn(true);
	dut -> trace(m_trace,3);
	m_trace -> open("waveform.vcd");
	reset(10);

	int cnt =0;
    while(sim_flag){
		single_cycle();
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
	// in/out port update when clk is high
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

extern "C" void dpi_that_accesses_din(svLogic din) {
	if(din){
		m_trace -> close();
		delete m_trace;
		delete dut;
		exit(EXIT_SUCCESS);
	}
	return ;
	/*
    svScope scope = svGetScope();  // 获取当前作用域
	assert(scope);  // Check for nullptr if scope not found
    // 通过DPI函数访问din变量
    svLogic din_value;
    svGetScopeVariable(scope, "din", &din_value);

    // 在C++中处理din变量的值
    // 执行逻辑或返回结果
    // 例如，打印din变量的值
	if(din_value){
		m_trace -> close();
		delete m_trace;
		delete dut;
		exit(EXIT_SUCCESS);
	}
	return ;
	*/
}