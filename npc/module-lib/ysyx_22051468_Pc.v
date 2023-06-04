`include "INST_TYPE.v"


localparam WIDTH = 64;
module ysyx_22051468_Pc #(
    ResetValue = `PC_RESETVALUE
)   
(
    input clk,
    input rst_n,
    input jump_en,
    input [WIDTH-1:0] jump_addr,
    output reg [WIDTH-1:0] pc_o
);



always@(posedge clk) begin
    if(!rst_n) pc_o <= ResetValue;
    else if(jump_en) pc_o <= jump_addr;
    else pc_o <= pc_o + 4;
end


endmodule