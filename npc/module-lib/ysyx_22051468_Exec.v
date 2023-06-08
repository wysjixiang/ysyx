
// This DPI function will read "din"
import "DPI-C" context function void dpi_that_accesses_din(input din);

// This DPI function will read inst and pc
import "DPI-C" context function void get_pc (input logic [31:0] a);
import "DPI-C" context function void get_inst (input logic [31:0] a);

// This will read/ write mem
import "DPI-C" function void pmem_read(
  input longint raddr, output longint rdata, input logic ena);
import "DPI-C" function void pmem_write(
  input longint waddr, input longint wdata, input byte wmask, input logic ena);


`include "INST_TYPE.v"


module ysyx_22051468_Exec #(
    WIDTH = 64
)
(

//Display debug
    input clk,
    input rst_n,

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

// ComplexAlu module wire
wire is_complex_alu;
wire [WIDTH-1:0] ComAlu_out;

// Result need to write to Rd
wire [WIDTH-1:0] inst_I_data;

// load & store
wire [WIDTH-1:0] load_data;

// test!
wire test_sig;
assign test_sig = ~clk & rst_n;

`undef debug
`ifdef debug
always@(posedge test_sig) begin
    $display("\n");
    $display("Exec!\t");
    $display("PC = %x\t",pc);
    $display("branch_jump_en = %d\t branch_jump_addr = %x\t",branch_jump_en,branch_jump_addr_o);
    $display("write_en = %d\t",w_addr_en);
    $display("write_addr = %d\t",w_addr_o);
    $display("write_data = %x\t",w_data_o);
    $display("inst_type_i = %x\t",inst_type_i);
    $display("is_branch = %d\t",is_branch_i);
    $display("is_jal = %d\t",is_jal_i);
    $display("is_jalr = %d\t",is_jalr_i);
    $display("is_load = %d\t",is_load_i);
    $display("is_store = %d\t",is_store_i);
    $display("is_mul = %d\t",is_mul);
    $display("is_div = %d\t",is_div);
    $display("is_div = %d\t",is_div);
    $display("is_w = %d\t",is_W_i);
    $display("is_u = %d\t",is_U_i);
    $display("opcode = %x\t",GenAlu_opcode);
    $display("Complexop_1 = %x\t",rs1_data_i);
    $display("Complexop_2 = %x\t",rs2_data_i);
    $display("ComplexAlu_out = %x\t",ComAlu_out);
    $display("Genop_1 = %x\t",GenAlu_op1);
    $display("Genop_2 = %x\t",GenAlu_op2);
    $display("imm = %x\t",imm_i);
    $display("GenAlu_out = %x\t",GenAlu_out);
    $display("bran_op1 = %x\t", BranchAlu_op1);
    $display("bran_op2 = %x\t",BranchAlu_op2);
    $display("explicit_op = %x\t",explicit_type_i);
end
`endif

// DPI-C communication 
// ********************************//
wire is_ebreak;
assign is_ebreak = 32'h00100073 == inst_i;
always @(*) begin
    dpi_that_accesses_din(is_ebreak);
    get_pc(pc[31:0]);
    get_inst(inst_i);
end

// wire of mem
wire [63:0] raddr;
wire [63:0] rdata;
wire [63:0] waddr;
wire [63:0] wdata;
wire [7:0] wmask;
wire mem_read_ena;
wire mem_write_ena;
wire [63:0] rdata_process;
// assign
// take care! this is a simulation code .
// need change for real RTL code
assign raddr = (rs1_data_i + imm_i);
assign mem_read_ena = is_load_i;
assign mem_write_ena = is_store_i;
assign waddr = (rs1_data_i + imm_i);
assign wdata = (rs2_data_i);
assign rdata_process = 
    rdata >> raddr[2:0] * 8
    ;

assign wmask = 
    ({8{(explicit_type_i[`SD])}} & 8'b11111111 ) | 
    ({8{(explicit_type_i[`SW])}} & 8'b00001111 ) | 
    ({8{(explicit_type_i[`SH])}} & 8'b00000011 ) | 
    ({8{(explicit_type_i[`SB])}} & 8'b00000001 )  
    ;

always @(negedge clk) begin
  pmem_read(raddr, rdata,mem_read_ena);
  pmem_write(waddr, wdata, wmask,mem_write_ena);
end


//*****************************//


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
assign is_complex_alu = is_mul | is_div | is_rem;

assign load_data = 
    ({WIDTH{(explicit_type_i[`LD])}} & rdata_process ) | 
    ({WIDTH{(explicit_type_i[`LW]) & is_U_i}} & {{(WIDTH/2){1'b0}},rdata_process[31:0]}) | 
    ({WIDTH{(explicit_type_i[`LW]) & !is_U_i}} & {{(WIDTH/2){rdata_process[31]}},rdata_process[31:0]}) | 
    ({WIDTH{(explicit_type_i[`LH]) & is_U_i}} & {{(WIDTH-16){1'b0}},rdata_process[15:0]}) | 
    ({WIDTH{(explicit_type_i[`LH]) & !is_U_i}} & {{(WIDTH-16){rdata_process[15]}},rdata_process[15:0]}) | 
    ({WIDTH{(explicit_type_i[`LB]) & is_U_i}} & {{(WIDTH-8){1'b0}},rdata_process[7:0]}) | 
    ({WIDTH{(explicit_type_i[`LB]) & !is_U_i}} & {{(WIDTH-8){rdata_process[7]}},rdata_process[7:0]})  
    ;

assign w_addr_en = rd_need_i;
assign w_addr_o = rd_addr_i;
assign w_data_o =  
    ({WIDTH{(is_jal_i | is_jalr_i)}} & (pc + 4)) |
    ({WIDTH{GenAlu_ena & !is_jal_i & !is_jalr_i & !is_load_i}} & GenAlu_out)  |
    ({WIDTH{(inst_type_i == `INST_U_LUI)}} & imm_i)     |
    ({WIDTH{(is_load_i)}} & load_data)  |
    ({WIDTH{is_complex_alu}} & ComAlu_out)  
    // add more
    ;

// GenAlu module assign
assign GenAlu_ena = 
    (!is_complex_alu)  & (
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
    (({WIDTH{rs2_need_i & !imm_need_i}}) & rs2_data_i)  
    ;

//assign GenAlu_opcode = 
//    ({(`EXPLICIT_TYPE_NUM){(inst_type_i == `INST_I_ || inst_type_i == `INST_U_AUIPC)}} & explicit_type_i)   |
//    ({(`EXPLICIT_TYPE_NUM){(inst_type_i == `INST_B_)}} & ({{(`EXPLICIT_TYPE_NUM-1){1'b0}},1'b1}))   |
//    ({(`EXPLICIT_TYPE_NUM){is_jal_i}} & ({{(`EXPLICIT_TYPE_NUM-1){1'b0}},1'b1}))|  
//    ({(`EXPLICIT_TYPE_NUM){is_jalr_i}} & ({{(`EXPLICIT_TYPE_NUM-1){1'b0}},1'b1}))   
//    ;

wire opcode_equal_explicit;
assign opcode_equal_explicit = 
    !is_branch_i
    // add more
    ;

assign GenAlu_opcode = 
    ({(`EXPLICIT_TYPE_NUM){!opcode_equal_explicit}} & ({{(`EXPLICIT_TYPE_NUM-1){1'b0}},1'b1}))   |
    ({(`EXPLICIT_TYPE_NUM){opcode_equal_explicit}} & explicit_type_i)   
    ;


ysyx_22051468_GenAlu    #(
	.WIDTH(WIDTH),
	.ALU_OPCODE_WIDTH(`EXPLICIT_TYPE_NUM)
)   GenAlu0(
	.GenAlu_op1      (GenAlu_op1),
	.GenAlu_op2      (GenAlu_op2),
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


ysyx_22051468_ComplexAlu_OneCycle #(
    .WIDTH(64),
	.ALU_OPCODE_WIDTH(`EXPLICIT_TYPE_NUM)
)   ComplexAlu0(
	.ComplexAlu_op1     (rs1_data_i),
	.ComplexAlu_op2     (rs2_data_i),
	.opcode             (explicit_type_i),
    .is_W_i             (is_W_i),
	.out_result         (ComAlu_out)
);

endmodule

