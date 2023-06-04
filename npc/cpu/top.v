
`include "INST_TYPE.v"

module top (
    input clk,
    input rst_n,
    input [`INST_WIDTH-1:0] inst_rom,
    output [WIDTH-1:0] inst_addr2rom,
    output [WIDTH*REG_NUM-1:0] GPR2TOP
);

localparam WIDTH = 64;
localparam REG_WIDTH = 5;
localparam REG_NUM = 32;


wire [WIDTH-1:0] pc_o;


// Controller
wire branch_jump_en_2con;
wire [WIDTH-1:0] branch_jump_addr_2con;
wire hold_pipeline_en_2con;
// Controller to seperate modules
wire branch_jump_en_2module;
wire [WIDTH-1:0] branch_jump_addr_2module;
wire hold_pipeline_en_2module;



ysyx_22051468_Pc #(
    .ResetValue(`PC_RESETVALUE)
)   PC0(
    .clk        (clk),
    .rst_n      (rst_n),
    .jump_en    (branch_jump_en_2module),
    .jump_addr  (branch_jump_addr_2module),
    .pc_o       (pc_o)
);


ysyx_22051468_Ifetch_Dff #(
    .WIDTH(WIDTH)
)   Ifetch0(
    .clk(clk),
    .rst_n(rst_n),
    .inst_i(inst_rom),
    .inst_i_addr(pc_o),
    .hold_pipeline      (hold_pipeline_en_2module),
    .addr_2rom(inst_addr2rom),
    .inst_o(inst_fromIfetch),
    .inst_o_addr(inst_addr_fromIfetch)
);

// wire 
// from Inst_decode
wire [REG_WIDTH-1:0] rs1_addr_from_instdec   ;
wire [REG_WIDTH-1:0] rs2_addr_from_instdec    ;
// to Inst_decode_dff
wire [WIDTH-1:0] rs1_data_2decdff       ;
wire [WIDTH-1:0] rs2_data_2decdff       ;
// from exec
wire [REG_WIDTH-1:0] rd_waddr   ;
wire [WIDTH-1:0] rd_wdata       ;
wire wen                        ;

ysyx_22051468_Reg #(
    .WIDTH(WIDTH)
)	GPR0(
    .clk(clk),
    .rst_n(rst_n),
    // from Inst_decode
    .rs1_raddr_i(rs1_addr_from_instdec),
    .rs2_raddr_i(rs2_addr_from_instdec),
    // to Inst_decode_dff
    .rs1_data(rs1_data_2decdff),
    .rs2_data(rs2_data_2decdff),
    // from exec
    .rd_waddr_i(rd_waddr),
    .rd_wdata_i(rd_wdata),
    .wen(wen),
    .GPR2TOP(GPR2TOP)
);


// wire
// from Ifetch_Dff
wire [`INST_WIDTH-1:0] inst_fromIfetch;
wire [WIDTH-1:0] inst_addr_fromIfetch;
// to Dff
wire [`INST_WIDTH-1:0] inst_2decdff;
wire [WIDTH-1:0] inst_addr_2decdff;
// gpr & imm related
wire [WIDTH-1:0] imm_2decdff;
wire [4:0] rd_addr_2decdff;
// need sig
wire rd_need_2decdff;
wire rs1_need_2decdff;
wire rs2_need_2decdff;
wire imm_need_2decdff;
// inst decode info
wire is_load_2decdff;
wire is_store_2decdff;
wire [3:0] inst_type_o_2decdff;
wire is_branch_2decdff;
wire is_jal_2decdff;
wire is_jalr_2decdff;
wire is_U_2decdff;
wire is_W_2decdff;
wire is_mul_2decdff         ;
wire is_div_2decdff         ;
wire is_rem_2decdff         ;
// define the explicit_type with one hot coding
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_type_2decdff;


ysyx_22051468_InstDecode #(
    .WIDTH(WIDTH)
)	INSTDECODE0(
    // from Ifetch_Dff
    .inst_i(inst_fromIfetch),
    .inst_addr_i(inst_addr_fromIfetch),
    // to regs
    .rs1_addr_o(rs1_addr_from_instdec),
    .rs2_addr_o(rs2_addr_from_instdec),
    // to Dff
    .inst_o(inst_2decdff),
    .inst_addr_o(inst_addr_2decdff),
    // gpr & imm related
    .imm_o(imm_2decdff),
    .rd_addr_o(rd_addr_2decdff),
    // need sig
    .rd_need(rd_need_2decdff),
    .rs1_need(rs1_need_2decdff),
    .rs2_need(rs2_need_2decdff),
    .imm_need(imm_need_2decdff),
    // Bus related!
    // need to inform bus one cycle ahead
    .is_load            (is_load_2decdff),
    .is_store           (is_store_2decdff),
    // inst decode info
    .inst_type_o        (inst_type_o_2decdff),
    .is_branch          (is_branch_2decdff),
    .is_jal             (is_jal_2decdff),
    .is_jalr            (is_jalr_2decdff),
    .is_U               (is_U_2decdff),
    .is_W               (is_W_2decdff),
    .is_mul             (is_mul_2decdff),
    .is_div             (is_div_2decdff),
    .is_rem             (is_rem_2decdff),
    // define the explicit_type with one hot coding
    .explicit_type_o    (explicit_type_2decdff)
);


// wire
// to exec
wire [WIDTH-1:0] rs1_data_2_exec;
wire [WIDTH-1:0] rs2_data_2_exec;
wire [`INST_WIDTH-1:0] inst_2exec;
wire [WIDTH-1:0] inst_addr_2exec;
// need sig to exec
wire rd_need_2exec;
wire rs1_need_2exec;
wire rs2_need_2exec;
wire imm_need_2exec;
// gpr & imm related
wire [WIDTH-1:0] imm_2exec;
wire [4:0] rd_addr_2exec;
// inst decode info
wire is_load_2exec;
wire is_store_2exec;
// inst decode info
wire [3:0] inst_type_2exec;
wire is_branch_2exec  ;
wire is_jal_2exec     ;
wire is_jalr_2exec    ;
wire is_U_2exec       ;
wire is_W_2exec       ;
wire is_mul_2exec         ;
wire is_div_2exec         ;
wire is_rem_2exec         ;
// define the explicit_type with one hot coding
wire [`EXPLICIT_TYPE_NUM-1:0] explicit_type_2exec;

ysyx_22051468_InstDecode_Dff #(
    .WIDTH(WIDTH)
)   INST_DFF0(
    .clk   (clk),
    .rst_n   (rst_n),
    // from InstDecode
    .inst_i   (inst_2decdff),
    .inst_addr_i   (inst_addr_2decdff),
    // from Regs
    .rs1_data_i(rs1_data_2decdff),
    .rs2_data_i(rs2_data_2decdff),
    // need sig
    .rd_need_i    (rd_need_2decdff),
    .rs1_need_i   (rs1_need_2decdff),
    .rs2_need_i   (rs2_need_2decdff),
    .imm_need_i   (imm_need_2decdff),
    // gpr & imm related
    .imm_i   (imm_2decdff),
    .rd_addr_i   (rd_addr_2decdff),
    // Bus related!
    // need to inform bus one cycle ahead
    .is_load_i            (is_load_2decdff),
    .is_store_i           (is_store_2decdff),
    // inst decode info
    .inst_type_i        (inst_type_o_2decdff),
    .is_branch_i          (is_branch_2decdff),
    .is_jal_i             (is_jal_2decdff),
    .is_jalr_i            (is_jalr_2decdff),
    .is_U_i               (is_U_2decdff),
    .is_W_i               (is_W_2decdff),
    .is_mul_i            (is_mul_2decdff),
    .is_div_i            (is_div_2decdff),
    .is_rem_i            (is_rem_2decdff),
    // define the explicit_type with one hot coding
    .explicit_type_i    (explicit_type_2decdff),
    .hold_pipeline      (hold_pipeline_en_2module),

    // to exec
    .rs1_data_o(rs1_data_2_exec),
    .rs2_data_o(rs2_data_2_exec),
    .inst_o   (inst_2exec),
    .inst_addr_o   (inst_addr_2exec),
    // need sig
    .rd_need_o    (rd_need_2exec),
    .rs1_need_o   (rs1_need_2exec),
    .rs2_need_o   (rs2_need_2exec),
    .imm_need_o   (imm_need_2exec),
    // gpr & imm related
    .imm_o   (imm_2exec),
    .rd_addr_o(rd_addr_2exec),
    // Bus related!
    // need to inform bus one cycle ahead
    .is_load_o            (is_load_2exec),
    .is_store_o           (is_store_2exec),
    // inst decode info
    .inst_type_o        (inst_type_2exec),
    .is_branch_o          (is_branch_2exec),
    .is_jal_o             (is_jal_2exec),
    .is_jalr_o            (is_jalr_2exec),
    .is_U_o               (is_U_2exec),
    .is_W_o               (is_W_2exec),
    .is_mul_o             (is_mul_2exec),
    .is_div_o             (is_div_2exec),
    .is_rem_o             (is_rem_2exec),
    // define the explicit_type with one hot coding
    .explicit_type_o    (explicit_type_2exec)
);


// Controller
wire branch_jump_en;
wire [WIDTH-1:0] branch_jump_addr_o;
wire hold_pipeline_en;

ysyx_22051468_Exec #(
    .WIDTH(WIDTH)
)   Exec0(
// input
    // from decdff
    .rs1_data_i        (rs1_data_2_exec),
    .rs2_data_i        (rs2_data_2_exec),
    .inst_i            (inst_2exec),
    .inst_addr_i       (inst_addr_2exec),
    // need sig
    .rd_need_i          (rd_need_2exec),
    .rs1_need_i         (rs1_need_2exec),
    .rs2_need_i         (rs2_need_2exec),
    .imm_need_i         (imm_need_2exec),
    // gpr & imm related
    .imm_i             (imm_2exec),
    .rd_addr_i         (rd_addr_2exec),
    // inst decode info
    .is_load_i          (is_load_2exec),
    .is_store_i         (is_store_2exec),
    // inst decode info
    // inst type indicating which type. I or U ..
    .inst_type_i       (inst_type_2exec),
    .is_branch_i         (is_branch_2exec),
    .is_jal_i            (is_jal_2exec),
    .is_jalr_i           (is_jalr_2exec),
    .is_U_i              (is_U_2exec),   
    .is_W_i              (is_W_2exec),   
    .is_mul              (is_mul_2exec), 
    .is_div              (is_div_2exec), 
    .is_rem              (is_rem_2exec), 
    // define the explicit_type with one hot coding
    // for submodule use
    .explicit_type_i      (explicit_type_2exec),

// output
    // write back
    .w_addr_o(rd_waddr),
    .w_data_o(rd_wdata),
    .w_addr_en     (wen),
// Controller
    .branch_jump_en         (branch_jump_en_2con    ),
    .branch_jump_addr_o     (branch_jump_addr_2con),
    .hold_pipeline_en       (hold_pipeline_en_2con  )

);


ysyx_22051468_Control #(
    .WIDTH(64)
)   Contoller0(
    .jump_en_i           (branch_jump_en_2con  ),
    .jump_addr_i         (branch_jump_addr_2con),
    .hold_pipeline_i     (hold_pipeline_en_2con),
    .jump_en_o           (branch_jump_en_2module  ),
    .jump_addr_o         (branch_jump_addr_2module),
    .hold_pipeline_o     (hold_pipeline_en_2module)
);




endmodule