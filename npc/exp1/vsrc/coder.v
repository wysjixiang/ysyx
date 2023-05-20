
module encoder83(

input [7:0] in_code,
input en,
output flag,
output reg [2:0] out_code

);


wire [7:0] prio;

assign prio[7] = in_code[7];
assign prio[6] = in_code[6] & ~in_code[7];
assign prio[5] = in_code[5] & ~in_code[6] & ~in_code[7];
assign prio[4] = in_code[4] & ~in_code[5] & ~in_code[6] & ~in_code[7];
assign prio[3] = in_code[3] & ~in_code[4] & ~in_code[5] & ~in_code[6] & ~in_code[7];
assign prio[2] = in_code[2] & ~in_code[3] & ~in_code[4] & ~in_code[5] & ~in_code[6] & ~in_code[7];
assign prio[1] = in_code[1] & ~in_code[2] & ~in_code[3] & ~in_code[4] & ~in_code[5] & ~in_code[6] & ~in_code[7];
assign prio[0] = in_code[0] & ~in_code[1] & ~in_code[2] & ~in_code[3] & ~in_code[4] & ~in_code[5] & ~in_code[6] & ~in_code[7];

assign flag = !(in_code == 0) & en;


always@(*) begin
	if(en) begin
		case(prio)
			
			8'b1000_0000:
				out_code = 3'd7;
			8'b0100_0000:
				out_code = 3'd6;
			8'b0010_0000:
				out_code = 3'd5;
			8'b0001_0000:
				out_code = 3'd4;
			8'b0000_1000:
				out_code = 3'd3;
			8'b0000_0100:
				out_code = 3'd2;
			8'b0000_0010:
				out_code = 3'd1;		
			default:
				out_code = 3'd0;
		
		endcase
	end else
	
		out_code = 3'h0;

end

endmodule 
