module top (
    input clk,
    input rst,
    input [7:0] sw,
    input ps2_clk,
    input ps2_data,
    output reg [15:0] ledr,
    output VGA_CLK,
    output VGA_HSYNC,
    output VGA_VSYNC,
    output VGA_BLANK_N,
    output [7:0] VGA_R,
    output [7:0] VGA_G,
    output [7:0] VGA_B,
    output [7:0] seg0,
    output [7:0] seg1,
    output [7:0] seg2,
    output [7:0] seg3,
    output [7:0] seg4,
    output [7:0] seg5,
    output [7:0] seg6,
    output [7:0] seg7
);

assign VGA_CLK		= 'h0;
assign VGA_HSYNC	= 'h0;
assign VGA_VSYNC	= 'h0;
assign VGA_BLANK_N = 'h0;
assign VGA_R = 'h0;
assign VGA_G = 'h0;
assign VGA_B = 'h0;
assign seg0 = 'h0;
assign seg1 = 'h0;
assign seg2 = 'h0;
assign seg3 = 'h0;
assign seg4 = 'h0;
assign seg5 = 'h0;
assign seg6 = 'h0;
assign seg7 = 'h0;

// switch
// assign ledr = {15'b0,sw[0] ^ sw[1]};

// 4-1 mux
always@(*) begin
	case(sw[7:6])
		2'b00: ledr = {15'h0,sw[0]};
		
		2'b01:ledr = {15'h0,sw[1]};
		
		2'b10:ledr = {15'h0,sw[2]};
		
		2'b11:ledr = {15'h0,sw[3]};

	endcase
end


endmodule
