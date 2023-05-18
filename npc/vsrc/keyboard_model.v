//`timescale 1ns/1ps


module keyboard_model(
	output reg ps2_clk,
	output reg ps2_data
		
);

parameter [31:0] kbd_clk_period = 60;
initial ps2_clk = 1'b1;

task key_sendcode;

	input [7:0] code;
	integer i;
	reg [10:0] sendbuffer;
	
	begin
		sendbuffer[0] = 1'b0;
		sendbuffer[8:1] = code;
		sendbuffer[9] = ~(^code);
		sendbuffer[10] = 1'b1;
		
		i=0;
		while(i<11) begin

			ps2_data = sendbuffer[i];
			#(kbd_clk_period/2) ps2_clk = 1'b0;
			#(kbd_clk_period/2) ps2_clk = 1'b1;
			i = i+1;
		end

	end
	
endtask


endmodule
