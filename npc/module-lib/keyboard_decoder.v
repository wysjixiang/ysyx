
module keyboard_decoder(

	input clk,
	input rst_n,
	input valid,
	input [7:0] data_in,
	
	output ready,
	output	[7:0] asci_data,
//	output 	reg [7:0] break_code,
//	output	reg [7:0] make_code,
	output [7:0] data_out,
	output [7:0] key_cnt,
	output keypressed,
	output shift,
	output ctrl,
	output caps
	
);


// Keyboard
localparam key_0 	= 8'h45;
localparam key_1 	= 8'h16;
localparam key_2 	= 8'h1E;
localparam key_3 	= 8'h26;
localparam key_4 	= 8'h25;
localparam key_5 	= 8'h2E;
localparam key_6 	= 8'h36;
localparam key_7 	= 8'h3D;
localparam key_8 	= 8'h3E;
localparam key_9 	= 8'h46;
localparam key_a 	= 8'h1C;
localparam key_b 	= 8'h32;
localparam key_c 	= 8'h21;
localparam key_d 	= 8'h23;
localparam key_e 	= 8'h24;
localparam key_f 		= 8'h2B;
localparam key_g 	= 8'h34;
localparam key_h 	= 8'h33;
localparam key_i 		= 8'h43;
localparam key_j 		= 8'h3B;
localparam key_k 	= 8'h42;
localparam key_l 		= 8'h4B;
localparam key_m 	= 8'h3A;
localparam key_n 	= 8'h31;
localparam key_o 	= 8'h44;
localparam key_p 	= 8'h4D;
localparam key_q 	= 8'h15;
localparam key_r  	= 8'h2D;
localparam key_s  	= 8'h1B;
localparam key_t  	= 8'h2C;
localparam key_u  	= 8'h3C;
localparam key_v 		= 8'h2A;
localparam key_w 	= 8'h1D;
localparam key_x 		= 8'h22;
localparam key_y 	= 8'h35;
localparam key_z 	= 8'h1A;



// reg & wire
wire ready_for_data;
wire uppercase;
reg [7:0] key_data;
reg on_process;

// cnt state
reg key_state;
reg [5:0] addr;
reg [39:0] key_reg;
reg [7:0] cnt;


// state machine
reg [3:0] state, n_state;
localparam s_nocombie 	= 4'b0000;	// it's perfect to use grey code for coding
localparam s_shift 		  	= 4'b0001;	//0x12
localparam s_ctrl				= 4'b0010;	//0x14
localparam s_caps			= 4'b0100;	//0x58
localparam s_shift_ctrl		= 4'b0011; 
localparam s_break_shift 		  	= 4'b1001;
localparam s_break_ctrl				= 4'b1010;
localparam s_break_caps			= 4'b1100;
localparam s_break_shift_ctrl		= 4'b1011; 

localparam key_shift 	= 8'h12;
localparam key_ctrl 		= 8'h14;
localparam key_caps 	= 8'h58;
localparam key_break 	= 8'hf0;


// assign 
assign ready = 1'b1;
assign ready_for_data = valid & ready;
assign shift = state[0];
assign ctrl = state[1];
assign caps = state[2];
assign data_out = key_data;
assign keypressed = |key_reg;
assign key_cnt = cnt;
assign uppercase = state[0] | state[2];

always@(posedge clk or negedge rst_n) begin
	key_data <= key_data;
	on_process <= 1'b0;
	if(!rst_n) begin
		key_data <= 8'd0;
		on_process <= 1'b0;
	end
	else if(ready_for_data) begin
		key_data <= data_in;
		on_process <= 1'b1;
	end
end


// state machine
always@(posedge clk or negedge rst_n) begin
	if(!rst_n) state <= s_nocombie;
	else state <= n_state;

end

/* verilator lint_off CASEINCOMPLETE */
// state transaction
always@(*) begin

	n_state = state;
	
	case(state)
		s_nocombie	: begin
			if(ready_for_data) begin
				case(data_in)
					key_shift: n_state = s_shift;
					
					key_ctrl: n_state =	s_ctrl;
					
					key_caps: n_state = s_caps;
					
					key_break: n_state = state;

				endcase
			end
		end
		
		s_shift	: begin
			if(ready_for_data) begin
				case(data_in)
					
					key_ctrl: n_state =	s_shift_ctrl;

					key_break: n_state = s_break_shift;

				endcase
			end
		end
		
		s_ctrl	: begin
			if(ready_for_data) begin
				case(data_in)
					key_shift: n_state = s_shift_ctrl;

					key_break: n_state = s_break_ctrl;

				endcase
			end
		end
		
		s_caps	: begin
			if(ready_for_data) begin
				case(data_in)
					
					key_break: n_state = s_break_caps;

				endcase
			end
		end

		s_shift_ctrl	: begin
			if(ready_for_data) begin
				case(data_in)

					key_break: n_state = s_break_shift_ctrl;

				endcase
			end
		end

		s_break_shift	: begin
			if(ready_for_data) begin
				case(data_in)
				
					key_shift: n_state = s_nocombie;

				endcase
			end
		end

		s_break_ctrl	: begin
			if(ready_for_data) begin
				case(data_in)
		
					key_ctrl: n_state =	s_nocombie;

				endcase
			end
		end

		s_break_caps	: begin
			if(ready_for_data) begin
				case(data_in)

					key_caps: n_state = s_nocombie;
					
				endcase
			end
		end

		s_break_shift_ctrl	: begin
			if(ready_for_data) begin
				case(data_in)
					key_shift: n_state = s_ctrl;
					
					key_ctrl: n_state = s_shift;

				endcase
			end
		end
		
	endcase


end
/* verilator lint_off CASEINCOMPLETE */

always@(posedge clk or negedge rst_n) begin

	key_reg <= key_reg;
	cnt <= cnt;
	key_state <= key_state;	


	if(!rst_n) begin
		key_reg <= 40'd0;
		cnt <= 8'd0;
		key_state <= 1'b0;	// wait for 0xf0;
	end else if(on_process) begin
		case(key_state)
			1'b0: begin
				if(addr == 6'd39) key_state <= 1'b1;
				else if(addr != 6'd50) begin
					key_reg[addr] <= 1'b1;
				end
			
			end

			1'b1: begin
				if(addr == 6'd50 ) key_state <= 1'b0;
				else if(addr != 6'd39 && key_reg[addr] == 1'b1) begin
					key_state <= 1'b0;
					key_reg[addr] <= 1'b0;
					cnt <= cnt + 1'b1;
				end
			end
		endcase
	end

end

// encoder for key
always@(*) begin
	case(key_data)
		key_0			:	addr = 6'd0  	;
		key_1			:	addr = 6'd1  	;
		key_2			:	addr = 6'd2  	;
		key_3			:	addr = 6'd3  	;
		key_4			:	addr = 6'd4  	;
		key_5			:	addr = 6'd5  	;
		key_6			:	addr = 6'd6  	;
		key_7			:	addr = 6'd7  	;
		key_8			:	addr = 6'd8  	;
		key_9			:	addr = 6'd9  	;
		key_a			:	addr = 6'd10	;
		key_b			:	addr = 6'd11	;
		key_c			:	addr = 6'd12	;
		key_d			:	addr = 6'd13	;
		key_e			:	addr = 6'd14	;
		key_f 			:	addr = 6'd15	;
		key_g			:	addr = 6'd16	;
		key_h			:	addr = 6'd17	;
		key_i 			:	addr = 6'd18	;
		key_j 			:	addr = 6'd19	;
		key_k			:	addr = 6'd20	;
		key_l 			:	addr = 6'd21	;
		key_m		:	addr = 6'd22	;
		key_n			:	addr = 6'd23	;
		key_o			:	addr = 6'd24	;
		key_p			:	addr = 6'd25	;
		key_q			:	addr = 6'd26	;
		key_r 			:	addr = 6'd27	;
		key_s			:	addr = 6'd28	;
		key_t 			:	addr = 6'd29	;
		key_u			:	addr = 6'd30	;
		key_v			:	addr = 6'd31	;
		key_w			:	addr = 6'd32	;
		key_x			:	addr = 6'd33	;
		key_y			:	addr = 6'd34	;
		key_z			:	addr = 6'd35	;
		key_shift  	:	addr = 6'd36	;
		key_ctrl		:	addr = 6'd37	;
		key_caps 		:	addr = 6'd38	;
		key_break	:	addr = 6'd39	;
		
		default			: addr = 6'd50;

	endcase
end

key2asci key2asci_inst(
	.data_in		(key_data),
	.uppercase	(uppercase),
	.data_out		(asci_data)
);


endmodule
