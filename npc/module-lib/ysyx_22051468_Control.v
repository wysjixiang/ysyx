

module ysyx_22051468_Control #(
    WIDTH = 64
)
(
    input jump_en_i,
    input [WIDTH-1:0] jump_addr_i,
    input hold_pipeline_i,

    output jump_en_o,
    output [WIDTH-1:0] jump_addr_o,
    output hold_pipeline_o
);



assign jump_en_o        =   jump_en_i;
assign jump_addr_o      =   jump_addr_i;
assign hold_pipeline_o  =   hold_pipeline_i;

endmodule