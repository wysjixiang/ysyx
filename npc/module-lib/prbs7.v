
module prbs7(
	input clk,
	input reset,
	output [7:0] out
);



reg [7:0] shift;

wire new_shift;
assign new_shift = shift[4] ^ shift[3] ^ shift[2] ^ shift[0] ;

assign out = shift;

always@(posedge clk or posedge reset) begin

	if(reset == 1) shift <= 8'd1;
	else shift <= { new_shift, shift[7:1]};

end


endmodule
