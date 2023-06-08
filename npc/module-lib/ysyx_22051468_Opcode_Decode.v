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
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_B_GenAlu;
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_LOAD_GenAlu;
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_STORE_GenAlu;
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_code_MDR_GenAlu; // mul & div & rem
// add more




// decode info
assign is_load = op_i == `INST_I_LOAD;
assign is_store = op_i == `INST_S_;
assign inst_type_o = op_i;
assign is_branch = op_i == `INST_B_;
assign is_jal = op_i == `INST_J_JAL;
assign is_jalr = op_i == `INST_I_JALR;

// is_U
// mul need special process! so not take the consider for mul & div & rem
assign is_U = 
    (op_i == `INST_I_ && (funct3 == 3'b011)) |
    (op_i == `INST_R_ && (funct3 == 3'b011 && funct7 == 7'b0)) |    // sltu 
    (op_i == `INST_B_ && (funct3 == 3'b110 || funct3 == 3'b111)) |
    (is_load && (funct3[2] == 1'b1) )   |
    (is_div && (funct3[0] == 1'b1))    |
    (is_rem && (funct3[0] == 1'b1))    
    // add more
    ;
// is_W
assign is_W = 
    (op_i == `INST_I_W) |
    (op_i == `INST_R_W) 
    ;

// todo() : how to handle mulh mulhu mulhsu ???
assign is_mul = 
    ((op_i == `INST_R_) && (funct3[2] == 1'b0) && (funct7 == 7'b1)) |
    ((op_i == `INST_R_W) && (funct3[2] == 1'b0) && (funct7 == 7'b1)) 
    ;
assign is_div = 
    ((op_i == `INST_R_) && (funct3[2:1] == 2'b10) && (funct7 == 7'b1)) |
    ((op_i == `INST_R_W) && (funct3[2:1] == 2'b10) && (funct7 == 7'b1)) 
    ;
assign is_rem = 
    ((op_i == `INST_R_) && (funct3[2:1] == 2'b11) && (funct7 == 7'b1)) |
    ((op_i == `INST_R_W) && (funct3[2:1] == 2'b11) && (funct7 == 7'b1)) 
    ;

// explicit type
assign explicit_type_o = 
    ({(`EXPLICIT_TYPE_NUM){(is_jal | is_jalr)}} & `EXPLICIT_TYPE_NUM'b1 )   | // make jump inst to sub in GenAlu module
    explicit_code_I_GenAlu |
    explicit_code_R_GenAlu |
    explicit_code_B_GenAlu |
    explicit_code_LOAD_GenAlu |
    explicit_code_STORE_GenAlu |
    explicit_code_MDR_GenAlu
    ;

// The one hot coding is in accordance with that
// in INST_TYPE.v

// add command compatible with auipc
// INST_I_ & INST_I_W
assign explicit_code_I_GenAlu[0] = ((op_i == `INST_I_ || op_i == `INST_I_W) && funct3 == 3'b000) | (op_i == `INST_U_AUIPC);
assign explicit_code_I_GenAlu[1] = (op_i == `INST_I_ && funct3 == 3'b111);
assign explicit_code_I_GenAlu[2] = 0;
assign explicit_code_I_GenAlu[3] = (op_i == `INST_I_ && funct3 == 3'b110);
assign explicit_code_I_GenAlu[4] = (op_i == `INST_I_ && funct3 == 3'b100);
assign explicit_code_I_GenAlu[5] = (op_i == `INST_I_ && funct3[2:1] == 2'b01);
assign explicit_code_I_GenAlu[6] = ((op_i == `INST_I_ || op_i == `INST_I_W) && funct3 == 3'b001 && funct7[6:1] == 6'b0);
assign explicit_code_I_GenAlu[7] = ((op_i == `INST_I_ || op_i == `INST_I_W) && funct3 == 3'b101 && funct7[6:1] == 6'b0);
assign explicit_code_I_GenAlu[8] = ((op_i == `INST_I_ || op_i == `INST_I_W) && funct3 == 3'b101 && funct7[6:1] == 6'b010000);

// INST_R_ & INST_R_W
assign explicit_code_R_GenAlu[0] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b000 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[1] = (op_i == `INST_R_ && funct3 == 3'b111 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[2] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b000 && funct7 == 7'b0100000) ;
assign explicit_code_R_GenAlu[3] = (op_i == `INST_R_ && funct3 == 3'b110 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[4] = (op_i == `INST_R_ && funct3 == 3'b100 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[5] = (op_i == `INST_R_ && funct3[2:1] == 2'b01 && funct7 == 7'b0);
assign explicit_code_R_GenAlu[6] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b001 && funct7 == 7'b0) ;
assign explicit_code_R_GenAlu[7] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b101 && funct7 == 7'b0);
assign explicit_code_R_GenAlu[8] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b101 && funct7 == 7'b0100000);


// INST_B_
assign explicit_code_B_GenAlu[0] = (op_i == `INST_B_ && funct3 == 3'b000);
assign explicit_code_B_GenAlu[1] = (op_i == `INST_B_ && funct3 == 3'b001);
assign explicit_code_B_GenAlu[2] = (op_i == `INST_B_ && (funct3 == 3'b100 || funct3 == 3'b110));
assign explicit_code_B_GenAlu[3] = (op_i == `INST_B_ && (funct3 == 3'b101 || funct3 == 3'b111));
assign explicit_code_B_GenAlu[4] = 1'b0;
assign explicit_code_B_GenAlu[5] = 1'b0;
assign explicit_code_B_GenAlu[6] = 1'b0;
assign explicit_code_B_GenAlu[7] = 1'b0;
assign explicit_code_B_GenAlu[8] = 1'b0;


// INST_I_LOAD
assign explicit_code_LOAD_GenAlu[0] = (op_i == `INST_I_LOAD && funct3 == 3'b011);
assign explicit_code_LOAD_GenAlu[1] = (op_i == `INST_I_LOAD && funct3[1:0] == 2'b10);
assign explicit_code_LOAD_GenAlu[2] = (op_i == `INST_I_LOAD && funct3[1:0] == 2'b01);
assign explicit_code_LOAD_GenAlu[3] = (op_i == `INST_I_LOAD && funct3[1:0] == 2'b00);
assign explicit_code_LOAD_GenAlu[4] = 1'b0;
assign explicit_code_LOAD_GenAlu[5] = 1'b0;
assign explicit_code_LOAD_GenAlu[6] = 1'b0;
assign explicit_code_LOAD_GenAlu[7] = 1'b0;
assign explicit_code_LOAD_GenAlu[8] = 1'b0;


// INST_S
assign explicit_code_STORE_GenAlu[0] = (op_i == `INST_S_ && funct3[1:0] == 2'b11);
assign explicit_code_STORE_GenAlu[1] = (op_i == `INST_S_ && funct3[1:0] == 2'b10);
assign explicit_code_STORE_GenAlu[2] = (op_i == `INST_S_ && funct3[1:0] == 2'b01);
assign explicit_code_STORE_GenAlu[3] = (op_i == `INST_S_ && funct3[1:0] == 2'b00);
assign explicit_code_STORE_GenAlu[4] = 1'b0;
assign explicit_code_STORE_GenAlu[5] = 1'b0;
assign explicit_code_STORE_GenAlu[6] = 1'b0;
assign explicit_code_STORE_GenAlu[7] = 1'b0;
assign explicit_code_STORE_GenAlu[8] = 1'b0;

// MUL & DIV & REM
assign explicit_code_MDR_GenAlu[0] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b000 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[1] = (op_i == `INST_R_ && funct3 == 3'b001 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[2] = (op_i == `INST_R_ && funct3 == 3'b011 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[3] = (op_i == `INST_R_ && funct3 == 3'b010 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[4] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b100 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[5] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b101 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[6] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b110 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[7] = ((op_i == `INST_R_ || op_i == `INST_R_W) && funct3 == 3'b111 && funct7 == 7'b1);
assign explicit_code_MDR_GenAlu[8] = 0;


endmodule
