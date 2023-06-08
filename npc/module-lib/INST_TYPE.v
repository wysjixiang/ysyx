
`define INST_WIDTH 32
`define PC_RESETVALUE 64'h80000000
// some special inst
`define INST_NOP 32'h00000013

// opcode tag
`define INST_TYPE_NUM 11
// U type
`define INST_U_AUIPC    4'b0001
`define INST_U_LUI      4'b0010
// I type
`define INST_I_         4'b0011
`define INST_I_LOAD     4'b0100
`define INST_I_W        4'b0101
`define INST_I_JALR     4'b0110
// J type
`define INST_J_JAL      4'b0111
// R type
`define INST_R_         4'b1000
`define INST_R_W        4'b1001
// B type
`define INST_B_         4'b1010
// S type
`define INST_S_         4'b1011
// EBREAK
`define INST_EBREAK     4'b1100


// care about the bits order!!
// INST_TYPE roughly 
`define TYPE_U_AUIPC    7'b0010111
`define TYPE_U_LUI      7'b0110111

`define TYPE_I_         7'b0010011
`define TYPE_I_LOAD     7'b0000011
`define TYPE_I_W        7'b0011011
`define TYPE_I_JALR     7'b1100111

`define TYPE_J_JAL      7'b1101111

`define TYPE_R_         7'b0110011
`define TYPE_R_W        7'b0111011

`define TYPE_B_         7'b1100011

`define TYPE_S_         7'b0100011

// ebreak
`define TYPE_EBREAK     7'b1110011



// define explicit inst type for exec
`define EXPLICIT_TYPE_NUM   9

//General ALU including genalu_W
`define ADD        0
`define AND        1
`define SUB        2
`define OR         3
`define XOR        4
`define SLT        5
`define SLL        6
`define SRL        7
`define SRA        8

// Branch ALU
`define BEQ        0
`define BNQ        1
`define BLT        2
`define BGE        3

// Mul & Div & Rem ALU


// Load ALU
`define LD        0
`define LW        1
`define LH        2
`define LB        3


// Store ALU
`define SD        0
`define SW        1
`define SH        2
`define SB        3

// Ebreak