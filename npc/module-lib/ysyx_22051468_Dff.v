
module ysyx_22051468_Dff #(
    WIDTH = 32,
    ResetValue = 0
)
(
    input clk,
    input rst_n,
    input wen,
    input [WIDTH-1:0] in,
    output reg [WIDTH-1:0] out
);

always@(posedge clk) begin
    if(!rst_n)
        out <= ResetValue;
    else if(wen)
        out <= in;
end

endmodule