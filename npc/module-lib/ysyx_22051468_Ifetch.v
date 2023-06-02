

module ysyx_22051468_Ifetch #(
   parameter WIDTH = 32
)
(
    input [WIDTH-1:0] pc_addr,
    input [WIDTH-1:0] inst_from_rom,
    output [WIDTH-1:0] addr2rom,
    output [WIDTH-1:0] inst_o,
    output [WIDTH-1:0] inst_addr
);

assign addr2rom = pc_addr;
assign inst_o = inst_from_rom;
assign inst_addr = pc_addr;


endmodule