`include "INST_TYPE.v"

module ysyx_22051468_Opcode_Decode  (
    input [3:0] op_i,
    input [2:0] funct3,
    input [6:0] funct7,
    // inst decode info
    output is_load,
    output is_store,
    output [3:0] inst_type_o,
    output is_branch,
    output is_jal,
    output is_jalr,
    output is_U,
    output is_W,
    output is_mul,
    output is_div,
    output is_rem,
    output [`EXPLICIT_TYPE_NUM-1:0] explicit_type_o
);

wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_I_GenAlu;
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_R_GenAlu;
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_I_W_GenAlu;
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_R_W_GenAlu;
// add more

// decode info
assign is_load = op_i == `INST_I_LOAD;
assign is_store = op_i == `INST_S_;
assign inst_type_o = op_i;
assign is_branch = op_i == `INST_B_;
assign is_jal = op_i == `INST_J_JAL;
assign is_jalr = op_i == `INST_I_JALR;

// these two need more conditions
// modify later!
assign is_U = 0;
assign is_W = 0;
assign is_mul = 0;
assign is_div = 0;
assign is_rem = 0;



// explicit type
assign explicit_type_o = 
    explicit_code_I_GenAlu |
    explicit_code_R_GenAlu
    ;

// The one hot coding is in accordance with that
// in INST_TYPE.v
assign explicit_code_I_GenAlu[0] = (op_i == `INST_I_ && funct3 == 3'b000);
assign explicit_code_I_GenAlu[1] = (op_i == `INST_I_ && funct3 == 3'b111);
assign explicit_code_I_GenAlu[2] = 0;
assign explicit_code_I_GenAlu[3] = (op_i == `INST_I_ && funct3 == 3'b110);
assign explicit_code_I_GenAlu[4] = (op_i == `INST_I_ && funct3 == 3'b100);
assign explicit_code_I_GenAlu[5] = (op_i == `INST_I_ && funct3[2:1] == 2'b01);
assign explicit_code_I_GenAlu[6] = (op_i == `INST_I_ && funct3 == 3'b001 && funct7[6:1] == 6'b0);
assign explicit_code_I_GenAlu[7] = (op_i == `INST_I_ && funct3 == 3'b101 && funct7[6:1] == 6'b0);
assign explicit_code_I_GenAlu[8] = (op_i == `INST_I_ && funct3 == 3'b101 && funct7[6:1] == 6'b010000);

assign explicit_code_R_GenAlu[0] = (op_i == `INST_R_ && funct3 == 3'b000 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[1] = (op_i == `INST_R_ && funct3 == 3'b111 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[2] = (op_i == `INST_R_ && funct3 == 3'b000 && funct7 == 7'b0100000) ;
assign explicit_code_R_GenAlu[3] = (op_i == `INST_R_ && funct3 == 3'b110 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[4] = (op_i == `INST_R_ && funct3 == 3'b100 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[5] = (op_i == `INST_R_ && funct3[2:1] == 2'b01 && funct7 == 7'b0);
assign explicit_code_R_GenAlu[6] = (op_i == `INST_R_ && funct3 == 3'b001 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[7] = (op_i == `INST_R_ && funct3 == 3'b101 && funct7 == 7'b0);
assign explicit_code_R_GenAlu[8] = (op_i == `INST_R_ && funct3 == 3'b101 && funct7 == 7'b0100000);


// add more






endmodule
