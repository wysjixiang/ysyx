`include "INST_TYPE.v"

module ysyx_22051468_GenAlu	#(
	WIDTH = 64,
	ALU_OPCODE_WIDTH = 9
)
(
	input [WIDTH-1:0] op_1,
	input [WIDTH-1:0] op_2,
	input [ALU_OPCODE_WIDTH-1:0] opcode,
    input is_U_i       ,
    input is_W_i       ,
	input ena,
	output overflow,
	output zero,
	output carry,
	output slt_out,
	output [WIDTH-1:0] out_result
);


// add & sub add_sub_result
wire [WIDTH-1:0] t_no_cin;
wire [WIDTH-1:0] add_sub_result;
wire need_sub;

// assign 
assign need_sub = opcode[`SUB] | opcode[`SLT] ;

assign t_no_cin = {WIDTH{need_sub}} ^ op_2;
assign {carry,add_sub_result} = op_1 + t_no_cin + {{(WIDTH-1){1'b0}},need_sub};
assign overflow = (op_1[WIDTH-1] == t_no_cin[WIDTH-1]) && (add_sub_result[WIDTH-1] != op_1[WIDTH-1] );
assign slt_out =                                                                   
 (is_U_i & need_sub) ^ carry |                                                     
 (!is_U_i & need_sub) & (add_sub_result[WIDTH-1] ^ overflow); 

// wire for result
wire [WIDTH-1:0] add_out;
wire [WIDTH-1:0] and_out;
wire [WIDTH-1:0] sub_out;
wire [WIDTH-1:0] or_out;
wire [WIDTH-1:0] xor_out;
wire [WIDTH-1:0] shift_data;
wire LorR;
wire AorL;

// assign
assign add_out = add_sub_result;
assign and_out = op_1 & op_2;
assign sub_out = add_sub_result;
assign or_out	= op_1 | op_2;
assign xor_out	= op_1 ^ op_2;


assign LorR = opcode[`SLL];
assign AorL = opcode[`SRA];



// assign out_resutl
assign zero = !(| out_result);
assign out_result = 
	{WIDTH{opcode[`ADD]}} & add_out |
	{WIDTH{opcode[`AND]}} & and_out |
	{WIDTH{opcode[`SUB]}} & sub_out |
	{WIDTH{opcode[`OR ]}} & or_out |
	{WIDTH{opcode[`XOR]}} & xor_out |
	{WIDTH{(opcode[`SLL] | opcode[`SRL] | opcode[`SRA])}} &  shift_data	|
	{WIDTH{opcode[`SLT] & slt_out}} 
	;




ysyx_22051468_Shift_64  #(
    .WIDTH(WIDTH)
)	shift0(
    .data_i(op_1),
    .shamt(op_2[5:0]),
    .LorR(LorR),
    .AorL(AorL),
    .data_o(shift_data)
);



endmodule
