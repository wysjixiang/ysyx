/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <cpu/cpu.h>
#include <difftest-def.h>
#include <memory/paddr.h>

// `direction`为`DIFFTEST_TO_DUT`时, 获取REF的寄存器状态到`dut`;
// `direction`为`DIFFTEST_TO_REF`时, 设置REF的寄存器状态为`dut`;
//#define DIFFTEST_TO_DUT 0
//#define DIFFTEST_TO_REF 1


__EXPORT void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction) {

  int div = n/4;
  int rem = n%4;
  uint32_t *data_4 = buf;

  if(direction == DIFFTEST_TO_REF){
    for(int i=0;i<div;i++){
      paddr_write(addr,4,*data_4);
      addr +=4;
      data_4++;
    }
    uint8_t *data_1 = (uint8_t *)data_4;
    for(int i=0;i<rem;i++){
      paddr_write(addr,1,*data_1);
      addr++;
      data_1++;
    }
  } else{
    for(int i=0;i<div;i++){
      *data_4 = paddr_read(addr,4);
      addr +=4;
      data_4++;
    }
    uint8_t *data_1 = (uint8_t *)data_4;
    for(int i=0;i<rem;i++){
      *data_1 = paddr_read(addr,1);
      addr++;
      data_1++;
    }
  }
}

__EXPORT void difftest_regcpy(void *dut, bool direction) {
  uint64_t *data = dut;
  if(direction == DIFFTEST_TO_REF){
    for(int i=0;i<32;i++){
      cpu.gpr[i] = *data++;
    }
    cpu.pc = *data++;
  } else{
    for(int i=0;i<32;i++){
      *data++ = cpu.gpr[i];
    }
    *data++ = cpu.pc;
  }
}

__EXPORT void difftest_exec(uint64_t n) {
  cpu_exec(n);
}

__EXPORT void difftest_raise_intr(word_t NO) {
  assert(0);
}

__EXPORT void difftest_init(int port) {
  void init_mem();
  init_mem();
  /* Perform ISA dependent initialization. */
  init_isa();
}


/*
// 在DUT host memory的`buf`和REF guest memory的`addr`之间拷贝`n`字节,
// `direction`指定拷贝的方向, `DIFFTEST_TO_DUT`表示往DUT拷贝, `DIFFTEST_TO_REF`表示往REF拷贝
void difftest_memcpy(paddr_t addr, void *buf, size_t n, bool direction);
// `direction`为`DIFFTEST_TO_DUT`时, 获取REF的寄存器状态到`dut`;
// `direction`为`DIFFTEST_TO_REF`时, 设置REF的寄存器状态为`dut`;
void difftest_regcpy(void *dut, bool direction);
// 让REF执行`n`条指令
void difftest_exec(uint64_t n);
// 初始化REF的DiffTest功能
void difftest_init();
*/