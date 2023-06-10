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

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  riscv64_csr.csr[CSR_MCAUSE] = NO;
  riscv64_csr.csr[CSR_MEPC] = epc;


  // etrace!
  #ifdef CONFIG_ETRACE
    printf("\033[32mE_Trace! Exception occurs\033[0m @ %lx!\n",epc);
    printf("\033[31mException Number\033[0m: %lx, \033[31mMepc\033[0m = %lx\n",NO,epc);

  #endif

  return riscv64_csr.csr[CSR_MTVEC];
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}
