

// `direction`为`DIFFTEST_TO_DUT`时, 获取REF的寄存器状态到`dut`;
// `direction`为`DIFFTEST_TO_REF`时, 设置REF的寄存器状态为`dut`;
#define DIFFTEST_TO_DUT 0
#define DIFFTEST_TO_REF 1
#define BASE_ADDR 0x80000000
#define STACK_SPACE 0x1000000


// define
#define UART_ADDR 0xa00003f8
#define RTC_ADDR 0xa0000048

// I_TRACE
#define I_TRACE
#undef I_TRACE


// DIFF_TEST
#define DIFF_TEST
#undef DIFF_TEST

// ASM_INFO
#define ASM_INFO
#undef ASM_INFO