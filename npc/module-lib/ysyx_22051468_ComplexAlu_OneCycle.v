`include "INST_TYPE.v"

module ysyx_22051468_ComplexAlu_OneCycle #(
    WIDTH = 64,
	ALU_OPCODE_WIDTH = 9
)
(
	input [WIDTH-1:0] ComplexAlu_op1,
	input [WIDTH-1:0] ComplexAlu_op2,
	input [ALU_OPCODE_WIDTH-1:0] opcode,
    input is_W_i       ,
	output [WIDTH-1:0] out_result
);


// sign 
wire [2*WIDTH-1:0] mul_result;
wire [2*WIDTH-1:0] mulhu_result;
wire [2*WIDTH-1:0] mulhsu_result;

wire [WIDTH-1:0] div_result;
wire [WIDTH-1:0] divu_result;
wire [WIDTH-1:0] rem_result;
wire [WIDTH-1:0] remu_result;


// W sign
wire [WIDTH/2-1:0] w_op_1;
wire [WIDTH/2-1:0] w_op_2;
wire [WIDTH-1:0] mulw_pre;
wire [WIDTH/2-1:0] divw_pre;
wire [WIDTH/2-1:0] divuw_pre;
wire [WIDTH/2-1:0] remw_pre;
wire [WIDTH/2-1:0] remuw_pre;

wire [WIDTH-1:0] mulw_result;
wire [WIDTH-1:0] divw_result;
wire [WIDTH-1:0] divuw_result;
wire [WIDTH-1:0] remw_result;
wire [WIDTH-1:0] remuw_result;

assign w_op_1 = ComplexAlu_op1[31:0];
assign w_op_2 = ComplexAlu_op2[31:0];
assign mulw_pre = ($signed(w_op_1) * $signed(w_op_2));
assign divw_pre = ($signed(w_op_1) / $signed(w_op_2));
assign divuw_pre = ((w_op_1) / (w_op_2));
assign remw_pre = ($signed(w_op_1) % $signed(w_op_2));
assign remuw_pre = ((w_op_1) % (w_op_2));

assign mulw_result = {{WIDTH/2{mulw_pre[31]}},mulw_pre[31:0]};
assign divw_result = {{WIDTH/2{divw_pre[31]}},divw_pre[31:0]};
assign divuw_result = {{WIDTH/2{divuw_pre[31]}},divuw_pre[31:0]};
assign remw_result = {{WIDTH/2{remw_pre[31]}},remw_pre[31:0]};
assign remuw_result = {{WIDTH/2{remuw_pre[31]}},remuw_pre[31:0]};



// result assign
assign out_result = 
	(({WIDTH{opcode[`MUL] && !is_W_i}} & mul_result[WIDTH-1:0]) |
	({WIDTH{opcode[`MULH]}} & mul_result[2*WIDTH-1-:WIDTH]) |
	({WIDTH{opcode[`MULHU]}} & mulhu_result[2*WIDTH-1-:WIDTH]) |
	({WIDTH{opcode[`MULHSU]}} & mulhsu_result[2*WIDTH-1-:WIDTH]) |
	({WIDTH{opcode[`DIV] && !is_W_i}} & div_result) |
	({WIDTH{opcode[`DIVU] && !is_W_i}} & divu_result) |
	({WIDTH{opcode[`REM] && !is_W_i}} & rem_result) |
	({WIDTH{opcode[`REMU] && !is_W_i}} & remu_result) |

	({WIDTH{opcode[`MUL] && is_W_i}} & mulw_result) |
	({WIDTH{opcode[`DIV] && is_W_i}} & divw_result) |
	({WIDTH{opcode[`DIVU] && is_W_i}} & divuw_result) |
	({WIDTH{opcode[`REM] && is_W_i}} & remw_result) |
	({WIDTH{opcode[`REMU] && is_W_i}} & remuw_result)) 
	;


// assign

assign mul_result = $signed(ComplexAlu_op1) * $signed(ComplexAlu_op2);
assign mulhu_result = ComplexAlu_op1 * ComplexAlu_op2;
assign mulhsu_result = $signed(ComplexAlu_op1) * ComplexAlu_op2;

assign div_result = $signed(ComplexAlu_op1) / $signed(ComplexAlu_op2);

assign divu_result = ComplexAlu_op1 / ComplexAlu_op2;

assign rem_result = $signed(ComplexAlu_op1) % $signed(ComplexAlu_op2);

assign remu_result = ComplexAlu_op1 % ComplexAlu_op2;

endmodule