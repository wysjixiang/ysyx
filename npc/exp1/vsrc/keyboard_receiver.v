

module keyboard_receiver(

	input clk,
	input rst_n,
	input ps2_clk,
	input ps2_data,
	input ready,
	
	output [7:0] data,
	output valid,
	output odd_parity_error,
	output stop_error,
	output empty,
	output full,
	output reg overflow

);

/////////////// register & wire

// FIFO
reg [63:0] fifo;
reg [7:0] buffer;
reg [3:0] w_ptr,r_ptr;
reg [1:0] ps2_clk_sync;
reg sampling;		// delay 1 period for sampling
reg odd_parity;

// state machine
localparam s_idle = 12'b00_00000_00001;
localparam s_1 = 12'b00_00000_00010;
localparam s_2 = 12'b00_00000_00100;
localparam s_3 = 12'b00_00000_01000;
localparam s_4 = 12'b00_00000_10000;
localparam s_5 = 12'b00_00001_00000;
localparam s_6 = 12'b00_00010_00000;
localparam s_7 = 12'b00_00100_00000;
localparam s_8 = 12'b00_01000_00000;
localparam s_9 = 12'b00_10000_00000;
localparam s_10 = 12'b01_00000_00000;
localparam s_11 = 12'b10_00000_00000;

reg [11:0] state;
reg [11:0] n_state;


wire fall_edge;


// assign 
assign fall_edge = ps2_clk_sync[1] & ~ps2_clk_sync[0];

assign empty = (w_ptr == r_ptr);
assign full = (w_ptr[3] ^ r_ptr[3]) && (w_ptr[2:0] == r_ptr[2:0]);
assign valid = !empty;
assign odd_parity_error = (state == s_11) && !odd_parity;
assign stop_error = (state == s_11) && !ps2_data;
// assign data = fifo[((r_ptr[2:0])<<3) +:8 ];
assign data = fifo[((r_ptr[2:0]) * 8) +:8 ];

always@(posedge clk) begin
	ps2_clk_sync <= { ps2_clk_sync[0], ps2_clk};
	sampling <= fall_edge;
end


// state machine transaction
always@(posedge clk or negedge rst_n) begin
	if(!rst_n) state <= s_idle;
	else state <= n_state;
end


/* verilator lint_off CASEINCOMPLETE */
always@(*) begin
	
	n_state = state;

	case(state) 
		s_idle	: if(sampling && (!ps2_data))  n_state = s_1;
		
		s_1 		: if(sampling)  n_state = s_2;
		
		s_2 		: if(sampling)  n_state = s_3;
		
		s_3 		: if(sampling)  n_state = s_4;
		
		s_4 		: if(sampling)  n_state = s_5;
		
		s_5 		: if(sampling)  n_state = s_6;
		
		s_6 		: if(sampling)  n_state = s_7;
		
		s_7 		: if(sampling) n_state = s_8;
		
		s_8 		: if(sampling)  n_state = s_9;
		
		s_9 		: if(sampling)  n_state = s_10;
		
		s_10		: if(sampling) n_state = s_11;
		
		s_11		: n_state = s_idle;
	
	endcase
	
end
/* verilator lint_off CASEINCOMPLETE */

/* verilator lint_off CASEINCOMPLETE */
// buffer 
always@(posedge clk or negedge rst_n) begin

	buffer <= buffer;
	odd_parity <= odd_parity;

	if(!rst_n) begin
		buffer <= 8'd0;
		odd_parity <= 1'b0;
	end
	else if(sampling) begin
		case(state)
		
			s_idle	: begin
				buffer <= 8'd0;
				odd_parity <= 1'b0;
			
			end
			
			s_1 		: begin
				buffer[0] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
			
			s_2 		: begin
				buffer[1] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
			
			s_3 		: begin
				buffer[2] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
			
			s_4		: begin
				buffer[3] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
            
			s_5		: begin
				buffer[4] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
			
			s_6		: begin
				buffer[5] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
            
			s_7 		: begin
				buffer[6] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
            
			s_8 		: begin
				buffer[7] <= ps2_data;
				odd_parity <= odd_parity ^ ps2_data;
			end
            
			s_9		: begin
				odd_parity <= odd_parity ^ ps2_data;
			end
			
			
		endcase
	end
end
/* verilator lint_off CASEINCOMPLETE */


// FIFO
always@(posedge clk or negedge rst_n) begin
	
	fifo <= fifo;
	overflow <= 1'b0;
	if(!rst_n) begin
		fifo <= 64'd0;
		overflow <= 1'b0;	
	end
	if(state == s_11 && odd_parity && ps2_data) begin
		if(full) begin
			overflow <= 1'b1;
		end else begin
			fifo[((w_ptr[2:0])*8) +:8  ] <= buffer;
		end
	end
end


// w_ptr
always@(posedge clk or negedge rst_n) begin
	w_ptr <= w_ptr;
	
	if(!rst_n) w_ptr <= 4'd0;
	else if(state == s_11 && odd_parity && ps2_data && !full) w_ptr <= w_ptr + 1'b1;

end

// r_ptr
always@(posedge clk or negedge rst_n) begin
	r_ptr <= r_ptr;
	
	if(!rst_n) r_ptr <= 4'd0;
	else if(valid && ready) r_ptr <= r_ptr + 1'b1;
		
end


endmodule