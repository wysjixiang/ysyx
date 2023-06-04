`include "INST_TYPE.v"

module ysyx_22051468_InstDecode_Dff #(
    WIDTH = 64  
)   (
    input clk,
    input rst_n,
    // from Regs
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
    // inst decode info_in
    input is_load_i,
    input is_store_i,
    // inst decode info
    input [3:0] inst_type_i,
    input is_branch_i,
    input is_jal_i,
    input is_jalr_i,
    input is_U_i,
    input is_W_i,
    input is_mul_i,
    input is_div_i,
    input is_rem_i,
    // define the explicit_type with one hot coding
    input [`EXPLICIT_TYPE_NUM-1:0] explicit_type_i,
    input hold_pipeline,

    // to exec
    output [WIDTH-1:0] rs1_data_o,
    output [WIDTH-1:0] rs2_data_o,
    output [`INST_WIDTH-1:0] inst_o,
    output [WIDTH-1:0] inst_addr_o,
    // need sig
    output rd_need_o,
    output rs1_need_o,
    output rs2_need_o,
    output imm_need_o,
    // gpr & imm related
    output [WIDTH-1:0] imm_o,
    output [4:0] rd_addr_o,
    // inst decode info
    output is_load_o,
    output is_store_o,
    // inst decode info
    output [3:0] inst_type_o,
    output is_branch_o  ,
    output is_jal_o     ,
    output is_jalr_o    ,
    output is_U_o       ,
    output is_W_o       ,
    output is_mul_o,
    output is_div_o,
    output is_rem_o,
    // define the explicit_type with one hot coding
    output [`EXPLICIT_TYPE_NUM-1:0] explicit_type_o

);

wire en;
// NOP
assign en = ~hold_pipeline;

// the inst need to be NOP if en is low
// take care of the order!
ysyx_22051468_Dff #(WIDTH,0) dff0(clk,rst_n,en,rs1_data_i,rs1_data_o);
ysyx_22051468_Dff #(WIDTH,0) dff1(clk,rst_n,en,rs2_data_i,rs2_data_o);
ysyx_22051468_Dff #(`INST_WIDTH,`INST_NOP) dff2(clk,rst_n,en,inst_i,inst_o);
ysyx_22051468_Dff #(WIDTH,64'h80000000) dff3(clk,rst_n,en,inst_addr_i,inst_addr_o);

ysyx_22051468_Dff #(1,1'b1) dff4(clk,rst_n,en,rd_need_i,rd_need_o);
ysyx_22051468_Dff #(1,1'b1) dff5(clk,rst_n,en,rs1_need_i,rs1_need_o);
ysyx_22051468_Dff #(1,0) dff6(clk,rst_n,en,rs2_need_i,rs2_need_o);
ysyx_22051468_Dff #(1,1'b1) dff7(clk,rst_n,en,imm_need_i,imm_need_o);

ysyx_22051468_Dff #(WIDTH,64'b0) dff8(clk,rst_n,en,imm_i,imm_o);
ysyx_22051468_Dff #(5,0) dff9(clk,rst_n,en,rd_addr_i,rd_addr_o);


ysyx_22051468_Dff #(1,0) dff10(clk,rst_n,en,is_load_i,is_load_o);
ysyx_22051468_Dff #(1,0) dff11(clk,rst_n,en,is_store_i,is_store_o);
ysyx_22051468_Dff #(4,4'b0011) dff12(clk,rst_n,en,inst_type_i,inst_type_o);
ysyx_22051468_Dff #(1,0) dff13(clk,rst_n,en,is_branch_i,is_branch_o);
ysyx_22051468_Dff #(1,0) dff14(clk,rst_n,en,is_jal_i   ,is_jal_o   );
ysyx_22051468_Dff #(1,0) dff15(clk,rst_n,en,is_jalr_i  ,is_jalr_o  );
ysyx_22051468_Dff #(1,0) dff16(clk,rst_n,en,is_U_i     ,is_U_o     );
ysyx_22051468_Dff #(1,0) dff17(clk,rst_n,en,is_W_i     ,is_W_o     );
ysyx_22051468_Dff #(`EXPLICIT_TYPE_NUM,0) dff18(clk,rst_n,en,explicit_type_i,explicit_type_o);


ysyx_22051468_Dff #(1,0) dff19(clk,rst_n,en,is_mul_i     ,is_mul_o     );
ysyx_22051468_Dff #(1,0) dff20(clk,rst_n,en,is_div_i     ,is_div_o     );
ysyx_22051468_Dff #(1,0) dff21(clk,rst_n,en,is_rem_i     ,is_rem_o     );



endmodule

