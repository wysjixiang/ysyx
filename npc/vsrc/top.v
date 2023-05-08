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

assign seg4 = 8'hff;
assign seg5 = 8'hff;


wire ready;
wire valid;	
wire [7:0] key_data;

wire odd_parity_error		;
wire stop_error					;	
wire empty						;
wire full							;
wire overflow					;
wire shift							;
wire ctrl							;
wire caps		                    ;
wire [7:0] asci_data			;
wire [7:0] data_out			;			
wire [7:0] key_cnt			;


reg [10:0] cnt;
reg light;

assign ledr[0] = shift;
assign ledr[1] = ctrl;
assign ledr[2] = caps;
assign ledr[15] = odd_parity_error;
assign ledr[14] = stop_error;
assign ledr[13] = empty;
assign ledr[12] = full;
assign ledr[11] = overflow;

assign ledr[7] = ~btnr;
assign ledr[9] = light;

// clk has no driver???? always 0
// finally found: we drive clk in main.cpp!!!
always@(posedge ps2_clk or posedge btnr) begin
	if(btnr) begin
		cnt <= 11'd0;
		light <= 1'b0;
	
	end else if(cnt == 11'd30) begin
		cnt <= 11'd0;
		light <= ~light;
	
	end else begin
		light <= light;
		cnt <= cnt + 1'b1;
	
	end

end


keyboard_receiver keyboard_receiver_inst(
	.clk								(clk),
	.rst_n							(rst),
	.ps2_clk						(ps2_clk),
	.ps2_data						(ps2_data),
	.ready							(ready),
	.data								(key_data),
	.valid							(valid),
	.odd_parity_error			(odd_parity_error),
	.stop_error					(stop_error),
	.empty							(empty),	
	.full								(full),
	.overflow	                    (overflow)

);
	
keyboard_decoder keyboard_decoder_inst(

	.clk					(clk),
	.rst_n				(rst),
	.valid				(valid),
	.data_in			(key_data),
	.ready				(ready),
	.asci_data			(asci_data),
	.data_out			(data_out),
	.key_cnt			(key_cnt),
	.shift					(shift),
	.ctrl					(ctrl),
	.caps				(caps)

);	


// scan code
bcd bcd_scanlow(
	.in_bin(data_out[3:0]),
	.out_dec(seg0)
);

bcd bcd_scanhigh(
	.in_bin(data_out[7:4]),
	.out_dec(seg1)
);

// asci code
bcd bcd_asciow(
	.in_bin(asci_data[3:0]),
	.out_dec(seg2)
);

bcd bcd_ascihigh(
	.in_bin(asci_data[7:4]),
	.out_dec(seg3)
);

// cnt
bcd bcd_cntlow(
	.in_bin(key_cnt[3:0]),
	.out_dec(seg6)
);

bcd bcd_cnthigh(
	.in_bin(key_cnt[7:4]),
	.out_dec(seg7)
);


endmodule



// switch
// assign ledr = {15'b0,sw[0] ^ sw[1]};

// 4-1 mux
//always@(*) begin
//	case(sw[15:14])
//		2'b00: ledr = {15'h0,sw[0]};
//		
//		2'b01:ledr = {15'h0,sw[1]};
//		
//		2'b10:ledr = {15'h0,sw[2]};
//		
//		2'b11:ledr = {15'h0,sw[3]};
//
//	endcase
//end

//assign seg1 = 8'hff;
//assign seg2 = 8'hff;
// assign seg3 = 8'hff;
// assign seg4 = 8'hff;  
// assign seg5 = 8'hff;   
// assign seg6 = 8'hff;   
// assign seg7 = 8'hff;   
                  
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
