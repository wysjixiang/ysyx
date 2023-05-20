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

assign seg1 = 8'hff;
assign seg2 = 8'hff;
assign seg3 = 8'hff;
assign seg4 = 8'hff;  
assign seg5 = 8'hff;   
assign seg6 = 8'hff;   
assign seg7 = 8'hff;   


// switch
assign ledr = {14'b0,light};

reg [1:0] light;

// 4-1 mux
always@(*) begin
	case(sw[1:0])
		2'b00: 
			light = sw[3:2];
		2'b01: 
			light = sw[5:4];
		2'b10: 
			light = sw[7:6];
		2'b11: 
			light = sw[9:8];
	endcase
end



endmodule



                  
//wire [2:0] bin;
//assign ledr[2:0] = bin;
//
//encoder83 encoder_inst(
//	.in_code(sw[7:0]),
//	.en(sw[15]),
//	.flag(ledr[3]),
//	.out_code(bin)
//);

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

// wire [7:0] prbs;

// bcd bcd_high(
	// .in_bin(prbs[7:4]),
	// .out_dec(seg2)
// );

// bcd bcd_low(
	// .in_bin(prbs[3:0]),
	// .out_dec(seg1)
// );


// prbs7 prbs7_inst(
	// .clk(btnr),
	// .reset(btnu),
	// .out(prbs)
// );
