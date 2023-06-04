
// This DPI function will read "din"
import "DPI-C" context function void dpi_that_accesses_din(input din);

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
    output w_addr_en,
    // to controller
    output branch_jump_en,
    output [WIDTH-1:0] branch_jump_addr_o,
    output hold_pipeline_en
);


// DPI-C communication 
wire is_ebreak;
assign is_ebreak = 32'h00100073 == inst_i;
always @(*)
   dpi_that_accesses_din(is_ebreak);

//task dpi_din_args(input din);
//   /* verilator no_inline_task */
//   dpi_that_accesses_din();
//endtask


// program counter
wire [WIDTH-1:0] pc;

assign pc = inst_addr_i;


// GenAlu module wire
wire [WIDTH-1:0] GenAlu_op1;
wire [WIDTH-1:0] GenAlu_op2;
wire GenAlu_ena;
wire [WIDTH-1:0] GenAlu_out;
wire GenAlu_overflow;
wire [`EXPLICIT_TYPE_NUM-1:0] GenAlu_opcode;

// BranchAlu module wire
wire [WIDTH-1:0] BranchAlu_op1;
wire [WIDTH-1:0] BranchAlu_op2;
wire BranchAlu_ena;
wire branch_jump;

// Result need to write to Rd
wire [WIDTH-1:0] inst_I_data;


// test!
always@(*) begin
    $display("PC = %x\t",pc);
    $display("branch_jump_en = %d\t branch_jump_addr = %x\t",branch_jump_en,branch_jump_addr_o);
    $display("inst = %x\t",inst_i);
    $display("write_en = %d\t",w_addr_en);
    $display("write_addr = %d\t",w_addr_o);
    $display("write_data = %x\t",w_data_o);
    $display("inst_type_i = %x\t",inst_type_i);
    $display("is_jal = %d\t",is_jal_i);
    $display("is_jalr = %d\t",is_jalr_i);
    $display("opcode = %x\t",GenAlu_opcode);
    $display("op_1 = %x\t",GenAlu_op1);
    $display("op_2 = %x\t",GenAlu_op2);
    $display("GenAlu_out = %x\t",GenAlu_out);
    $display("\t",);
end

// to controller
assign branch_jump_en = 
    branch_jump | is_jal_i | is_jalr_i
    ;
assign hold_pipeline_en = 
    branch_jump | is_jal_i | is_jalr_i
    ;
assign branch_jump_addr_o = 
    ({WIDTH{GenAlu_ena}} & GenAlu_out)
    // add more

    ;

// write back to regs
// assign 
assign w_addr_en = rd_need_i;
assign w_addr_o = rd_addr_i;
assign w_data_o = 
    ({WIDTH{(is_jal_i | is_jalr_i)}} & (pc + 4)) |
    ({WIDTH{GenAlu_ena & !is_jal_i & !is_jalr_i}} & GenAlu_out)  |
    ({WIDTH{(inst_type_i == `INST_U_LUI)}} & imm_i)
    // add more
    ;

// GenAlu module assign
assign GenAlu_ena = 
    (!is_mul & !is_div & !is_rem)  & (
    (inst_type_i == `INST_U_AUIPC ) |
    (inst_type_i == `INST_I_ ) |
    (inst_type_i == `INST_I_W ) |
    (is_jalr_i ) |
    (inst_type_i == `INST_R_ ) |
    (inst_type_i == `INST_R_W ) |
    (inst_type_i == `INST_B_ ) |
    (is_jal_i) 
    );

assign GenAlu_op1 = 
    // auipc
    (({WIDTH{((inst_type_i == `INST_U_AUIPC) |
    (is_jal_i) |

    // branch
    (inst_type_i == `INST_B_))}}) & pc)    |
    // I and R 
    (({WIDTH{((inst_type_i == `INST_I_) |
    (inst_type_i == `INST_I_W) |
    (is_jalr_i) |
    (inst_type_i == `INST_R_) |
    (inst_type_i == `INST_R_W))}}) & rs1_data_i)    
    ;
assign GenAlu_op2 = 
    // imm
    // I and R 
    // B type
    // Jal
    //(({WIDTH{((inst_type_i == `INST_U_AUIPC) |
    //(inst_type_i == `INST_I_) |
    //(is_jal_i) |
    //(inst_type_i == `INST_B_) |
    //(is_jalr_i) |
    //(inst_type_i == `INST_I_W))}}) & imm_i) |
    //// src2
    //(({WIDTH{((inst_type_i == `INST_R_) |


    // test
    (({WIDTH{imm_need_i}}) & imm_i)  |
    (({WIDTH{rs2_need_i}}) & rs2_data_i)  
    ;

assign GenAlu_opcode = 
    ({(`EXPLICIT_TYPE_NUM){(inst_type_i == `INST_I_ | inst_type_i == `INST_U_AUIPC)}} & explicit_type_i)   |
    ({(`EXPLICIT_TYPE_NUM){(inst_type_i == `INST_B_)}} & ({{(`EXPLICIT_TYPE_NUM-1){1'b0}},1'b1}))   |
    ({(`EXPLICIT_TYPE_NUM){is_jal_i}} & ({{(`EXPLICIT_TYPE_NUM-1){1'b0}},1'b1}))|  
    ({(`EXPLICIT_TYPE_NUM){is_jalr_i}} & ({{(`EXPLICIT_TYPE_NUM-1){1'b0}},1'b1}))   
    ;


ysyx_22051468_GenAlu    #(
	.WIDTH(WIDTH),
	.ALU_OPCODE_WIDTH(`EXPLICIT_TYPE_NUM)
)   GenAlu0(
	.op_1            (GenAlu_op1),
	.op_2            (GenAlu_op2),
	.opcode          (GenAlu_opcode ),
    .is_U_i          (is_U_i)    ,
    .is_W_i          (is_W_i)    ,
    .ena             (GenAlu_ena),
	.overflow        (GenAlu_overflow),
	.zero            (),
	.carry           (),
    .slt_out         (),
	.out_result      (GenAlu_out)

);


// BranchAlu module assign
assign BranchAlu_ena = 
    (inst_type_i == `INST_B_)
    ;

assign BranchAlu_op1 = rs1_data_i;
assign BranchAlu_op2 = rs2_data_i;

ysyx_22051468_BranchAlu	#(
	.WIDTH(64),
	.ALU_OPCODE_WIDTH(`EXPLICIT_TYPE_NUM)
)   BranchAlu0(
	.op_1           (BranchAlu_op1),
	.op_2           (BranchAlu_op2),
	.opcode         (explicit_type_i),
    .is_U_i         (is_U_i),
	.ena            (BranchAlu_ena ),
	.branch_jump    (branch_jump)
);

endmodule

