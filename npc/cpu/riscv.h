

// `direction`为`DIFFTEST_TO_DUT`时, 获取REF的寄存器状态到`dut`;
// `direction`为`DIFFTEST_TO_REF`时, 设置REF的寄存器状态为`dut`;
#define DIFFTEST_TO_DUT 0
#define DIFFTEST_TO_REF 1
#define BASE_ADDR 0x80000000
#define STACK_SPACE 0x1000000


// define
#define UART_ADDR 0xa00003f8
#define RTC_ADDR 0xa0000048
// ref_skip index
#define REF_RUN 1
#define REF_SKIP_NOP 2
#define REF_SKIP_READ 3
#define REF_SKIP_WRITE 4

// I_TRACE
// DIFF_TEST
// ASM_INFO
#define I_TRACE
#define DIFF_TEST
#define ASM_INFO

// undefine
#undef I_TRACE
#undef DIFF_TEST
#undef ASM_INFO
