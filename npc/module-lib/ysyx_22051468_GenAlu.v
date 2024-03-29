`include "INST_TYPE.v"

module ysyx_22051468_GenAlu	#(
	WIDTH = 64,
	ALU_OPCODE_WIDTH = 9
)
(
	input [WIDTH-1:0] GenAlu_op1,
	input [WIDTH-1:0] GenAlu_op2,
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


wire [WIDTH-1:0] op_1;
wire [WIDTH-1:0] op_2;
wire is_op_w;
wire [5:0] shamt;

// add & sub add_sub_result
wire [WIDTH-1:0] t_no_cin;
wire [WIDTH-1:0] add_sub_result;
wire need_sub;

// assign 
assign shamt = 
	({6{is_W_i}} & {1'b0,op_2[4:0]})  |
	({6{!is_W_i}} & op_2[5:0])  
	;


assign is_op_w = 
	is_W_i & (opcode[`SRL] | opcode[`SRA])
	;

assign op_1 = 
	({WIDTH{is_op_w}} & ({{(WIDTH/2){opcode[`SRA] & GenAlu_op1[31]}},GenAlu_op1[31:0]}))|
	({WIDTH{!is_op_w}} & GenAlu_op1)
	;

// seems op_2 not need to consider is_w
assign op_2 = GenAlu_op2;

assign need_sub = opcode[`SUB] | opcode[`SLT] ;
assign t_no_cin = {WIDTH{need_sub}} ^ op_2;
assign {carry,add_sub_result} = op_1 + t_no_cin + {{(WIDTH-1){1'b0}},need_sub};
assign overflow = (op_1[WIDTH-1] == t_no_cin[WIDTH-1]) && (add_sub_result[WIDTH-1] != op_1[WIDTH-1] );
assign slt_out =                                                                   
 (is_U_i & (need_sub ^ carry)) |                                                     
 (!is_U_i  & (add_sub_result[WIDTH-1] ^ overflow)); 

// wire for result
wire [WIDTH-1:0] add_out;
wire [WIDTH-1:0] and_out;
wire [WIDTH-1:0] sub_out;
wire [WIDTH-1:0] or_out;
wire [WIDTH-1:0] xor_out;
wire [WIDTH-1:0] shift_data;
wire [WIDTH-1:0] result_pre;
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
assign result_pre  = 
	{WIDTH{opcode[`ADD]}} & add_out |
	{WIDTH{opcode[`AND]}} & and_out |
	{WIDTH{opcode[`SUB]}} & sub_out |
	{WIDTH{opcode[`OR ]}} & or_out |
	{WIDTH{opcode[`XOR]}} & xor_out |
	{WIDTH{(opcode[`SLL] | opcode[`SRL] | opcode[`SRA])}} &  shift_data	|
	{{(WIDTH-1){1'b0}},opcode[`SLT] & slt_out} 
	;

assign zero = !(| out_result);
assign out_result = 
	({WIDTH{is_W_i}} & {{(WIDTH/2){result_pre[31]}}, result_pre[31:0]}) |
	({WIDTH{!is_W_i}} & result_pre) 
	;




ysyx_22051468_Shift_64  #(
    .WIDTH(WIDTH)
)	shift0(
    .data_i(op_1),
    .shamt(shamt),
    .LorR(LorR),
    .AorL(AorL),
    .data_o(shift_data)
);



endmodule
