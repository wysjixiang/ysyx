//0号管脚： dot；
//1号管脚： middle row line;
//2号管脚： upper left column;
//3号管脚：	lower left column;
//4号管脚：	lower row line;
//5号管脚：	lower right column;
//6号管脚：	upper right column;
//7号管脚：	upper row line;
module bcd(
	input [3:0] in_bin,
	output reg [7:0] out_dec

);

always@(*) begin
	case(in_bin)
		4'd0: out_dec = 8'b0000_0011;
		4'd1:	out_dec = 8'b1001_1111;
		4'd2:	out_dec = 8'b0010_0101;
		4'd3:	out_dec = 8'b0000_1101;
		4'd4:	out_dec = 8'b1001_1001;
		4'd5:	out_dec = 8'b0100_1001;
		4'd6:	out_dec = 8'b0100_0001;
		4'd7:	out_dec = 8'b0001_1111;
		4'd8:	out_dec = 8'b0000_0001;
		4'd9:	out_dec = 8'b0000_1001;
		4'd10:out_dec = 8'b0001_0001;
		4'd11:out_dec = 8'b1100_0001;
		4'd12:out_dec = 8'b0110_0011;
		4'd13:out_dec = 8'b1000_0101;
		4'd14:out_dec = 8'b0110_0001;
		4'd15:out_dec = 8'b0111_0001;

	endcase

end




endmodule
