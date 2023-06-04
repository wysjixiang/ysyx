
`include "INST_TYPE.v"


module ysyx_22051468_BranchAlu	#(
	WIDTH = 64,
	ALU_OPCODE_WIDTH = 9
)
(
	input [WIDTH-1:0] op_1,
	input [WIDTH-1:0] op_2,
	input [ALU_OPCODE_WIDTH-1:0] opcode,
    input is_U_i       ,
	input ena,
	output branch_jump
);


// add & sub add_sub_result
wire [WIDTH-1:0] t_no_cin;
wire [WIDTH-1:0] add_sub_result;
wire need_sub;
wire carry;
wire overflow;
wire slt_out;


// assign 
assign need_sub = 1'b1;

assign t_no_cin = {WIDTH{need_sub}} ^ op_2;
assign {carry,add_sub_result} = op_1 + t_no_cin + {{(WIDTH-1){1'b0}},need_sub};
assign overflow = (op_1[WIDTH-1] == t_no_cin[WIDTH-1]) && (add_sub_result[WIDTH-1] != op_1[WIDTH-1] );

// assign

assign slt_out	= 
	(is_U_i & need_sub) ^ carry |
	(!is_U_i & need_sub) & (add_sub_result[WIDTH-1] ^ overflow);
assign branch_jump = ena & (
	(opcode[`BEQ] & (op_1 == op_2))		|
	(opcode[`BNQ] & !(op_1 == op_2))	|
	(opcode[`BLT] & slt_out)			|
	(opcode[`BGE] & !slt_out)		
	);


endmodule
