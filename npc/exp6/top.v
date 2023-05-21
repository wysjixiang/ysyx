module top (
    input clk,
    input rst,
    input [15:0] sw,
		input btnr,
		input btnu,
    input ps2_clk,
    input ps2_data,
    output [15:0] ledr,
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

assign VGA_CLK		= clk;
assign VGA_HSYNC	= 'h0;
assign VGA_VSYNC	= 'h0;
assign VGA_BLANK_N = 'h0;
assign VGA_R = 'h0;
assign VGA_G = 'h0;
assign VGA_B = 'h0;

assign seg0 = 8'hff;
assign seg1 = 8'hff;
assign seg2 = 8'hff;
assign seg3 = 8'hff;
assign seg4 = 8'hff;  
assign seg5 = 8'hff;   
assign seg6 = 8'hff;   
assign seg7 = 8'hff;   


//prbs7 prbs7_inst(
// .clk(btnr),
// .reset(btnu),
// .out(ledr[7:0])
//);


bsr8 bsr8_inst(
	.din		(sw[7:0])		,
	.shamt	(sw[15:13])		,
	.L_R		(sw[10])		,
	.A_L		(sw[9])		,
	.dout		(ledr[7:0])
);



endmodule	


								

// wire [2:0] op;
// assign op = sw[15:13];


// bcd bcd_inst(
	// .in_bin({1'b0,op}),
	// .out_dec(seg0)
// );

// alu alu_inst(
	// .in_a(sw[7:4]),
	// .in_b(sw[3:0]),
	// .opcode(op),
	// .overflow(ledr[6]),
	// .zero(ledr[15]),
	// .carry(ledr[4]),
	// .out_result(ledr[3:0])
// );



