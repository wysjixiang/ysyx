
module bsr8(
	input [7:0] din			,
	input [2:0] shamt	,
	input L_R				,
	input A_L				,
	output [7:0] dout		
);

reg [7:0] data;
assign dout = data;

always@(*) begin
	case(L_R)
		1'b0:
			case(A_L)
				1'b0:
					data = din >> shamt;
				1'b1:
					data = $signed(din) >>> shamt;
			endcase

		1'b1:
			data = din << shamt;
	endcase
end


endmodule
