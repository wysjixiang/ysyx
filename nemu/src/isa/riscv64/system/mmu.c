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
#include <memory/vaddr.h>
#include <memory/paddr.h>


#define BITMASK(bits) ((1ull << (bits)) - 1)
#define BITS(x, hi, lo) (((x) >> (lo)) & BITMASK((hi) - (lo) + 1)) // similar to x[hi:lo] in verilog
//enum { MMU_DIRECT, MMU_TRANSLATE, MMU_FAIL };
// import
uintptr_t get_csr_satp();

#define ppn_end 53
#define ppn_begin 10
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  paddr_t ret_paddr = 0;
  uintptr_t _satp = get_csr_satp();

  uintptr_t *page1= (uintptr_t *)(BITS(_satp,43,0) << 12);
  uintptr_t *page2 = NULL;
  uintptr_t *page3 = NULL;
  uintptr_t _va = vaddr;
  uint32_t offset = BITS(_va,11,0);
  uint32_t va0 = BITS(_va,20,12);
  uint32_t va1 = BITS(_va,29,21);
  uint32_t va2 = BITS(_va,38,30);


  uintptr_t *p1 = page1 + va2;
  uintptr_t p1_data = paddr_read((uintptr_t)p1,8);

  // check if this PTE is not available
  if((p1_data & 0xF) == 1 && (p1_data >> 63 == 1)){
    page2 = (uintptr_t *)(BITS(p1_data,ppn_end,ppn_begin) << 12);
  } else{
    printf("Error when search PTE-1 table\n");
    printf("_satp = %lx\n",_satp);
    printf("vaddr = %lx\n",vaddr);
    printf("page1_addr = %lx\n",(uintptr_t)page1);
    printf("p1 = %lx\n",(uintptr_t)p1);
    printf("p1_data = %lx\n",p1_data);
    printf("offset = %x\n",offset);
    printf("va0 = %x\n",va0);
    printf("va1 = %x\n",va1);
    printf("va2 = %x\n",va2);
    assert(0);
  }

  uintptr_t *p2 = page2 + va1;
  uintptr_t p2_data = paddr_read((uintptr_t)p2,8);
  // check if this PTE is not available
  if((p2_data & 0xF) == 1 && (p2_data >> 63 == 1)){
    page3 = (uintptr_t *)(BITS(p2_data,ppn_end,ppn_begin) << 12);
  } else{
    printf("Error when search PTE-2 table\n");
    printf("vaddr = %lx\n",vaddr);
    printf("_satp = %lx\n",_satp);
    printf("vaddr = %lx\n",vaddr);
    printf("page2_addr = %lx\n",(uintptr_t)page2);
    printf("p2 = %lx\n",(uintptr_t)p2);
    printf("p2_data = %lx\n",p2_data);
    printf("offset = %x\n",offset);
    printf("va0 = %x\n",va0);
    printf("va1 = %x\n",va1);
    printf("va2 = %x\n",va2);
    assert(0);
  }

  uintptr_t *p3 = page3 + va0;
  uintptr_t p3_data = paddr_read((uintptr_t)p3,8);
  // check if this PTE is not available
  if((p3_data & 0xF) == 0xF && (p3_data >> 63 == 1)){
    ret_paddr = (BITS(p3_data,ppn_end,ppn_begin) << 12) + offset;
  } else{
    printf("Error when search PTE-3 table\n");
    printf("_satp = %lx\n",_satp);
    printf("vaddr = %lx\n",vaddr);
    printf("page3_addr = %lx\n",(uintptr_t)page3);
    printf("p3 = %lx\n",(uintptr_t)p3);
    printf("p3_data = %lx\n",p3_data);
    printf("offset = %x\n",offset);
    printf("va0 = %x\n",va0);
    printf("va1 = %x\n",va1);
    printf("va2 = %x\n",va2);
    assert(0);
  }

  return ret_paddr;
}

int isa_mmu_check(vaddr_t vaddr, int len, int type){
  bool ret = 0;
  uintptr_t _satp = get_csr_satp();
  if((_satp >> 60) != 0){
    if(_satp >> 60 != 8){
      printf("not sv39 PTE\n"); 
      assert(0);
    }

    ret = MMU_TRANSLATE;   

  } else{
    ret = MMU_DIRECT;
  }
  return ret;
}