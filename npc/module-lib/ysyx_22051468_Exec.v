`include "INST_TYPE.v"

module ysyx_22051468_Exec #(
    WIDTH = 64
)
(
// input
    // from decdff
    input [WIDTH-1:0] rs1_data_i,
    input [WIDTH-1:0] rs2_data_i,
    input [`INST_WIDTH-1:0] inst_i,
    input [WIDTH-1:0] inst_addr_i,
    // need sig
    input rd_need_i,
    input rs1_need_i,
    input rs2_need_i,
    input imm_need_i,
    // gpr & imm related
    input [WIDTH-1:0] imm_i,
    input [4:0] rd_addr_i,
    // inst decode info
    input is_load_i,
    input is_store_i,
    // inst decode info
    // inst type indicating which type. I or U ..
    input [3:0] inst_type_i,
    input is_branch_i  ,
    input is_jal_i     ,
    input is_jalr_i    ,
    input is_U_i       ,
    input is_W_i       ,
    input is_mul       ,
    input is_div       ,
    input is_rem       ,
    // define the explicit_type with one hot coding
    // for submodule use
    input [`EXPLICIT_TYPE_NUM-1:0] explicit_type_i,

// output
    // write back
    output [4:0] w_addr_o,
    output [WIDTH-1:0] w_data_o,
    output w_addr_en

);


// program counter
wire [WIDTH-1:0] pc;

assign pc = inst_addr_i;


// GenAlu module wire
wire [WIDTH-1:0] GenAlu_op1;
wire [WIDTH-1:0] GenAlu_op2;
wire GenAlu_ena;
wire [WIDTH-1:0] GenAlu_out;
wire GenAlu_overflow;

// GenAlu module assign
assign GenAlu_ena = 
    (!is_mul & !is_div & !is_rem)   |
    (inst_type_i == `INST_U_AUIPC ) |
    (inst_type_i == `INST_I_ ) |
    (inst_type_i == `INST_I_W ) |
    (inst_type_i == `INST_R_ ) |
    (inst_type_i == `INST_R_W ) 
    ;

assign GenAlu_op1 = 
    (({WIDTH{(inst_type_i == `INST_U_AUIPC)}}) & pc) |
    (({WIDTH{((inst_type_i == `INST_I_) |
    (inst_type_i == `INST_I_W) |
    (inst_type_i == `INST_R_) |
    (inst_type_i == `INST_R_W))}}) & rs1_data_i)
    ;
assign GenAlu_op2 = 
    // imm
    (({WIDTH{((inst_type_i == `INST_U_AUIPC) |
    (inst_type_i == `INST_I_) |
    (inst_type_i == `INST_I_W))}}) & imm_i) |
    // src2
    (({WIDTH{((inst_type_i == `INST_R_) |
    (inst_type_i == `INST_R_W))}}) & rs2_data_i) 
    ;


// write back to regs
// assign 
assign w_addr_en = rd_need_i;
assign w_addr_o = rd_addr_i;
assign w_data_o = GenAlu_out;


ysyx_22051468_GenAlu    #(
	.WIDTH(WIDTH),
	.ALU_OPCODE_WIDTH(`EXPLICIT_TYPE_NUM)
)   GenAlu0(
	.op_1            (GenAlu_op1),
	.op_2            (GenAlu_op2),
	.opcode          (explicit_type_i),
    .is_U_i          (is_U_i)    ,
    .is_W_i          (is_W_i)    ,
    .ena             (GenAlu_ena),
	.overflow        (GenAlu_overflow),
	.zero            (),
	.carry           (),
    .slt_out         (),
	.out_result      (GenAlu_out)

);


endmodule

