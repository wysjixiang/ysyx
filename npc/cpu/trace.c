
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern "C" void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);

void read_inst(uint32_t *this_s);
void read_pc(uint64_t *this_pc);


static char str[100] = { 0 };
static int cnt =0;



void itrace(){
    uint64_t pc = 0;
    uint8_t s[4] = {0};
    uint32_t *s_inst_buf = (uint32_t *)s;
    read_pc(&pc);
    read_inst(s_inst_buf);
    disassemble(&str[0],96,pc,s,4);
    printf("Disasm Info:");
    printf("PC:0x%lx, Hex_code:0x%08x,  inst:%s\n",pc, *s_inst_buf ,str);
}
