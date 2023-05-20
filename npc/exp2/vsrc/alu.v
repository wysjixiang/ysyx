

module alu(
	input [width-1:0] in_a,
	input [width-1:0] in_b,
	input [2:0] opcode,
	output overflow,
	output zero,
	output carry,
	output reg [width-1:0] out_result

);
parameter width = 4;

wire [width-1:0] t_no_cin;
wire [width-1:0] result;
assign zero = !(| out_result);

assign t_no_cin = {width{opcode[0]}} ^ in_b;
assign {carry,result} = in_a + t_no_cin + {3'b0,opcode[0]};
assign overflow = (in_a[width-1] == t_no_cin[width-1]) && (result[width-1] != in_a[width-1] );


always@(*) begin
	case(opcode)
		3'd0: // add
			out_result = result; 

		3'd1:	// sub
			out_result = result;

		3'd2:	// not
			out_result = ~in_a;

		3'd3:	// and
			out_result = in_a & in_b;

		3'd4:	// or
			out_result = in_a | in_b;

		3'd5:	// xor
			out_result = in_a ^ in_b;

// let blt when opcode = 7 since 7 = 0111,so opcode[0] = 1,the sub module could be reused.
		3'd7:	// blt
			out_result = {3'b0, result[width-1] ^ overflow};

// let blt when opcode = 6
		3'd6:	// equ
			out_result = {3'b0, !(|(in_a ^ in_b))};

	endcase
end


endmodule
