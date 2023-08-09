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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

#define csr_index_mcause 0x342
#define csr_index_mstatus 0x300
#define csr_index_mepc 0x341
#define csr_index_mtvec 0x305
#define csr_index_satp 0x180
static uint16_t csr_index_csr[] =
{
  0x342,0x300,0x341,0x305,0x180
};


// extern func call or ret func from monitor.c
void FuncCallRet(int rd,int rs1, uint64_t addr, char type);
void mstatus_recovery();

enum {
  TYPE_I, TYPE_U, TYPE_S,
  TYPE_J, TYPE_R,
  TYPE_B, TYPE_N, TYPE_C
  // none
};

#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12; } while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7); } while(0)
// Add
// imm is multiplier of 2 in immJ
#define immJ() do {  *imm =  SEXT( \
(BITS(i, 31, 31) << 20) | \
(BITS(i, 19, 12) << 12) | \
(BITS(i, 20, 20) << 11) | \
(BITS(i, 30, 21) << 1), 21) ;    } while(0)
// imm is multiplier of 2 in immB
#define immB() do {  *imm =  SEXT( \
(BITS(i, 31, 31) << 12) | \
(BITS(i, 7, 7) << 11) | \
(BITS(i, 30,25) << 5) | \
(BITS(i, 11,8) << 1), 13) ;    } while(0)


static uint16_t csr_index = 0;
static uint64_t csr_val = 0;
void csrR(uint32_t inst){
  csr_index = BITS(inst,31,20);
  for(int i=0;i<(sizeof(csr_index_csr)/ sizeof(uint16_t));i++){
    if(csr_index == csr_index_csr[i]){
      csr_index = i;
      csr_val = riscv64_csr.csr[i];
      return ;
    }
  }
  printf("No valid csr!\n");
  printf("inst = %x\n",inst);
  assert(0);
}


static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_R: src1R(); src2R();         break;
    case TYPE_B: src1R(); src2R(); immB(); break;
    case TYPE_C: src1R(); csrR(i);         break;
    //TODO();
  }
}


#define INSTPAT_U() do { \
  /* U type inst   */ \
                      \
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", Auipc  , U, R(rd) = s->pc + imm); \
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm); \
} while(0)

#define INSTPAT_I() do { \
  /* I type inst   */ \
                      \
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm     );  \
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm     );  \
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , I, R(rd) = src1 | imm      ); \
  INSTPAT("??????? ????? ????? 100 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm     ); \
  INSTPAT("??????? ????? ????? 010 ????? 00100 11", slti   , I, \
    R(rd) = (int64_t)src1 < (int64_t)imm ? 1:0 );  \
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, \
    R(rd) = src1 < imm ? 1:0 ); \
  INSTPAT("000000? ????? ????? 001 ????? 00100 11", SLLI   , I,\
    R(rd) = src1 << BITS(imm,5,0) );\
  INSTPAT("000000? ????? ????? 101 ????? 00100 11", SRLI   , I,\
    R(rd) = src1 >> BITS(imm,5,0) );\
  INSTPAT("010000? ????? ????? 101 ????? 00100 11", SRAI   , I,\
    R(rd) = BITS(src1,63,63) ? (SEXT(1,1) << (64-BITS(imm,5,0)))| \
    (src1 >> BITS(imm,5,0)):\
     src1 >> BITS(imm,5,0)); \
  INSTPAT("??????? ????? ????? 011 ????? 00000 11", LD     , I, \
    R(rd) = Mr(src1 + imm, 8)); \
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", LW     , I, \
    R(rd) = SEXT(BITS(Mr(src1 + imm,4),31,0),32)  ); \
  INSTPAT("??????? ????? ????? 110 ????? 00000 11", LWU     , I, \
    R(rd) = Mr(src1 + imm,4) ); \
  INSTPAT("??????? ????? ????? 001 ????? 00000 11", LH     , I, \
    R(rd) = SEXT(BITS(Mr(src1 + imm,2),15,0),16)  ); \
  INSTPAT("??????? ????? ????? 101 ????? 00000 11", LHU     , I, \
    R(rd) = Mr(src1 + imm,2) ); \
  INSTPAT("??????? ????? ????? 000 ????? 00000 11", LB     , I, \
    R(rd) = SEXT(BITS(Mr(src1 + imm,1),7,0),8)  ); \
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", LBU     , I, \
    R(rd) = Mr(src1 + imm,1) ); \
  INSTPAT("??????? ????? ????? 000 ????? 00110 11", ADDIW   , I, \
    R(rd) = SEXT(BITS(src1 + imm,31,0) ,32)  );  \
  INSTPAT("0000000 ????? ????? 001 ????? 00110 11", SLLIW   , I,\
    R(rd) = SEXT(BITS(BITS(src1,31,0) << BITS(imm,4,0),31,0),32) );\
  INSTPAT("0000000 ????? ????? 101 ????? 00110 11", SRLIW   , I,\
    R(rd) = SEXT(BITS(BITS(src1,31,0) >> BITS(imm,4,0),31,0),32) );\
  INSTPAT("0100000 ????? ????? 101 ????? 00110 11", SRAIW   , I,\
    R(rd) = BITS(src1,31,31) ?  \
    (SEXT(BITS(src1,31,0),32) >> BITS(imm,4,0) ) | (SEXT(1,1) << 32)  :  \
    BITS(src1,31,0) >> BITS(imm,4,0)); \
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(rd) = s->snpc, \
    s->dnpc = src1 + imm, FuncCallRet(rd,BITS(s->isa.inst.val, 19, 15), s->dnpc , 'I') );\
} while(0)

#define INSTPAT_J() do { \
  /* J type inst   */ \
                      \
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(rd) = s->snpc , \
   s->dnpc = (s->dnpc)-4 + (int64_t)imm,FuncCallRet(rd,BITS(s->isa.inst.val,19,15), s->dnpc , 'J') );  \
                      \
} while(0)
#define INSTPAT_R() do { \
  /* R type inst   */ \
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", ADD    , R,R(rd) = src1 + src2);\
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    , R,R(rd) = src1 & src2);\
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", SUB    , R,R(rd) = src1 - src2);\
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R,R(rd) = src1 | src2);\
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R,R(rd) = src1 ^ src2);\
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R, \
    R(rd) = (int64_t)src1 < (int64_t)src2 ? 1:0); \
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   , R, \
    R(rd) = src1 < src2 ? 1:0 );\
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", SLL    , R, \
    R(rd) = src1 << (BITS(src2,5,0))); \
  INSTPAT("0000000 ????? ????? 101 ????? 01100 11", SRL    , R,\
    R(rd) = src1 >> (BITS(src2,5,0)));\
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", SRA    , R,\
    R(rd) = BITS(src1,63,63) ? (SEXT(1,1) << (64-BITS(src2,5,0)))| \
    (src1 >> BITS(src2,5,0)):\
     src1 >> BITS(src2,5,0)); \
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", MUL    , R, \
    R(rd) = (src1 * src2) ); \
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", MULH    , R, \
    R(rd) = (((__uint128_t)((int64_t)src1 * (int64_t)src2)) >> 64) ); \
  INSTPAT("0000001 ????? ????? 011 ????? 01100 11", MULHU    , R, \
    R(rd) = (((__uint128_t)(src1 * src2)) >> 64) ); \
  INSTPAT("0000001 ????? ????? 010 ????? 01100 11", MULHSU    , R, \
    R(rd) = (((__uint128_t)((int64_t)src1 * src2)) >> 64) ); \
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", DIV    , R, \
    R(rd) = ((int64_t)src1 / (int64_t)src2) ); \
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", DIVU    , R, \
    R(rd) = (src1 / src2) ); \
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", REM    , R, \
    R(rd) = ((int64_t)src1 % (int64_t)src2) ); \
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", REMU    , R, \
    R(rd) = (src1 % src2) ); \
  INSTPAT("0000000 ????? ????? 000 ????? 01110 11", ADDW    , R, \
    R(rd) = SEXT(BITS(src1,31,0) + BITS(src2,31,0) , 32)   );\
  INSTPAT("0100000 ????? ????? 000 ????? 01110 11", SUBW    , R, \
    R(rd) = SEXT(BITS(src1,31,0) - BITS(src2,31,0) , 32)   );\
  INSTPAT("0000000 ????? ????? 001 ????? 01110 11", SLLW    , R, \
    R(rd) = SEXT(BITS(src1,31,0) << BITS(src2,4,0) , 32) ); \
  INSTPAT("0000000 ????? ????? 101 ????? 01110 11", SRLW    , R, \
    R(rd) = SEXT(BITS(src1,31,0) >> BITS(src2,4,0) , 32) ); \
  INSTPAT("0100000 ????? ????? 101 ????? 01110 11", SRAW    , R, \
    R(rd) = BITS(src1,31,31) ?  \
    (SEXT(BITS(src1,31,0),32) >> BITS(src2,4,0) ) | (SEXT(1,1) << 32)  :  \
    BITS(src1,31,0) >> BITS(src2,4,0)); \
  INSTPAT("0000001 ????? ????? 000 ????? 01110 11", MULW    , R, \
    R(rd) = SEXT(BITS(((int32_t)BITS(src1,31,0) * (int32_t)BITS(src2,31,0)),31,0),32) ); \
  INSTPAT("0000001 ????? ????? 100 ????? 01110 11", DIVW    , R, \
    R(rd) = SEXT(BITS(((int32_t)BITS(src1,31,0) / (int32_t)BITS(src2,31,0)),31,0),32) ); \
  INSTPAT("0000001 ????? ????? 101 ????? 01110 11", DIVUW    , R, \
    R(rd) = SEXT(BITS(((uint32_t)BITS(src1,31,0) / (uint32_t)BITS(src2,31,0)),31,0),32) ); \
  INSTPAT("0000001 ????? ????? 110 ????? 01110 11", REMW    , R, \
    R(rd) = SEXT(BITS(((int32_t)BITS(src1,31,0) % (int32_t)BITS(src2,31,0)),31,0),32) ); \
  INSTPAT("0000001 ????? ????? 111 ????? 01110 11", REMUW    , R, \
    R(rd) = SEXT(BITS((BITS(src1,31,0) % BITS(src2,31,0)),31,0),32) ); \
} while(0)
#define INSTPAT_B() do { \
  /* B type inst   */ \
                      \
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, \
    s->dnpc = (src1 == src2) ?  s->dnpc -4 + imm  :  s->dnpc);\
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bnq    , B, \
    s->dnpc = (src1 != src2) ?  s->dnpc -4 + imm  :  s->dnpc);\
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, \
    s->dnpc = ((int64_t)src1 < (int64_t)src2) ?  s->dnpc -4 + imm  :  s->dnpc);\
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, \
    s->dnpc = (src1 < src2) ?  s->dnpc -4 + imm  :  s->dnpc);\
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, \
    s->dnpc = ((int64_t)src1 >= (int64_t)src2)?  s->dnpc -4 + imm  :  s->dnpc);\
  INSTPAT("??????? ????? ????? 111 ????? 11000 11", bgeu   , B, \
    s->dnpc = (src1 >= src2) ?  s->dnpc -4 + imm  :  s->dnpc);\
} while(0)
#define INSTPAT_S() do { \
  /* S type inst   */ \
                      \
  INSTPAT("??????? ????? ????? 011 ????? 01000 11", sd     , S, Mw(src1 + imm, 8, src2)); \
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2)); \
  INSTPAT("??????? ????? ????? 001 ????? 01000 11", sh     , S, Mw(src1 + imm, 2, src2)); \
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2)); \
} while(0)
#define INSTPAT_N() do { \
  /* N type inst   */ \
  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); \
  INSTPAT("0000000 00000 00000 000 00000 11100 11", ecall , N, s->dnpc = isa_raise_intr(gpr(17),s->pc)); \
  INSTPAT("0011000 00010 00000 000 00000 11100 11", mret , N, s->dnpc = riscv64_csr.csr[CSR_MEPC], mstatus_recovery()); \
  INSTPAT("??????? ????? ????? 001 ????? 11100 11", CSRRW , C, \
    riscv64_csr.csr[csr_index] = src1, R(rd) = csr_val); \
  INSTPAT("??????? ????? ????? 010 ????? 11100 11", CSRRS , C, \
    riscv64_csr.csr[csr_index] = csr_val | src1, R(rd) = csr_val); \
  INSTPAT("00001?? ????? ????? 011 ????? 01011 11", AMOSWAP , C, \
    riscv64_csr.csr[csr_index] = R(rd) ; uint64_t temp = csr_val; R(rd) = temp); \
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc)); \
} while(0)

// INSTPAT
#define InstPat() do{ \
  INSTPAT_U(); \
  INSTPAT_I(); \
  INSTPAT_J(); \
  INSTPAT_R(); \
  INSTPAT_B(); \
  INSTPAT_S(); \
  INSTPAT_N(); \
} while(0)


static int decode_exec(Decode *s) {
  int rd = 0;
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;

#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}
  INSTPAT_START();
  InstPat();
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}

