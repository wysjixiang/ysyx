`include "INST_TYPE.v"

module ysyx_22051468_Ifetch_Dff #(
    parameter WIDTH = 64
)
(
    input clk,
    input rst_n,
    input [`INST_WIDTH-1:0] inst_i,
    input [WIDTH-1:0] inst_i_addr,
    output [WIDTH-1:0] addr_2rom,
    output [`INST_WIDTH-1:0] inst_o,
    output [WIDTH-1:0] inst_o_addr
);

assign addr_2rom = inst_i_addr;


ysyx_22051468_Dff #(
    .WIDTH(`INST_WIDTH),
    .ResetValue(`INST_NOP)
)   i0(
   .clk    (clk)     ,
   .rst_n  (rst_n)     ,
   .wen     (1'b1)      ,
   .in     (inst_i)     ,
   .out    (inst_o)     
);

ysyx_22051468_Dff #(
    .WIDTH(WIDTH),
    .ResetValue(64'h0)
)   i1(
   .clk    (clk)     ,
   .rst_n  (rst_n)     ,
   .wen     (1'b1)      ,
   .in     (inst_i_addr)     ,
   .out    (inst_o_addr)     
);

endmodule