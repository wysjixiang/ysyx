
module key2asci(
	input [7:0] data_in,
	input uppercase,
	output reg [7:0] data_out
	
);


// Keyboard
// this is extended keyboard code
// localparam key_0 	= 8'h70;
// localparam key_1 	= 8'h69;
// localparam key_2 	= 8'h72;
// localparam key_3 	= 8'h7A;
// localparam key_4 	= 8'h6B;
// localparam key_5 	= 8'h73;
// localparam key_6 	= 8'h74;
// localparam key_7 	= 8'h6C;
// localparam key_8 	= 8'h75;
// localparam key_9 	= 8'h7D;

//
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
localparam key_f 	= 8'h2B;
localparam key_g 	= 8'h34;
localparam key_h 	= 8'h33;
localparam key_i 	= 8'h43;
localparam key_j 	= 8'h3B;
localparam key_k 	= 8'h42;
localparam key_l 	= 8'h4B;
localparam key_m 	= 8'h3A;
localparam key_n 	= 8'h31;
localparam key_o 	= 8'h44;
localparam key_p 	= 8'h4D;
localparam key_q 	= 8'h15;
localparam key_r  	= 8'h2D;
localparam key_s  	= 8'h1B;
localparam key_t  	= 8'h2C;
localparam key_u  	= 8'h3C;
localparam key_v 	= 8'h2A;
localparam key_w 	= 8'h1D;
localparam key_x 	= 8'h22;
localparam key_y 	= 8'h35;
localparam key_z 	= 8'h1A;
	
// ASCII
localparam asci_0 = 8'h30;
localparam asci_1 = 8'h31;
localparam asci_2 = 8'h32;
localparam asci_3 = 8'h33;
localparam asci_4 = 8'h34;
localparam asci_5 = 8'h35;
localparam asci_6 = 8'h36;
localparam asci_7 = 8'h37;
localparam asci_8 = 8'h38;
localparam asci_9 = 8'h39;

localparam asci_a 	= 8'h61		;
localparam asci_b 	= 8'h62		;
localparam asci_c 	= 8'h63		;
localparam asci_d 	= 8'h64		;
localparam asci_e 	= 8'h65		;
localparam asci_f 	= 8'h66		;
localparam asci_g 	= 8'h67		;
localparam asci_h 	= 8'h68		;
localparam asci_i 	= 8'h69		;
localparam asci_j 	= 8'h6A		;
localparam asci_k	= 8'h6B		;
localparam asci_l 	= 8'h6C		;
localparam asci_m 	= 8'h6D		;
localparam asci_n	= 8'h6E		;
localparam asci_o	= 8'h6F		;
localparam asci_p	= 8'h70		;
localparam asci_q	= 8'h71		;
localparam asci_r 	= 8'h72		;
localparam asci_s 	= 8'h73		;
localparam asci_t 	= 8'h74		;
localparam asci_u 	= 8'h75		;
localparam asci_v 	= 8'h76		;	
localparam asci_w 	= 8'h77		;
localparam asci_x 	= 8'h78		;
localparam asci_y 	= 8'h79		;
localparam asci_z 	= 8'h7A		;

localparam asci_A 	= 8'h41		;
localparam asci_B 	= 8'h42		;
localparam asci_C 	= 8'h43		;
localparam asci_D 	= 8'h44		;
localparam asci_E 	= 8'h45		;
localparam asci_F 	= 8'h46		;
localparam asci_G 	= 8'h47		;
localparam asci_H 	= 8'h48		;
localparam asci_I 	= 8'h49		;
localparam asci_J 	= 8'h4A		;
localparam asci_K 	= 8'h4B		;
localparam asci_L 	= 8'h4C		;
localparam asci_M	= 8'h4D		;
localparam asci_N 	= 8'h4E		;
localparam asci_O 	= 8'h4F		;
localparam asci_P 	= 8'h50		;
localparam asci_Q 	= 8'h51		;
localparam asci_R 	= 8'h52		;
localparam asci_S 	= 8'h53		;
localparam asci_T 	= 8'h54		;
localparam asci_U 	= 8'h55		;
localparam asci_V 	= 8'h56		;
localparam asci_W	= 8'h57		;
localparam asci_X 	= 8'h58		;
localparam asci_Y 	= 8'h59		;
localparam asci_Z 	= 8'h5A		;

localparam key_shift 	= 8'h12;
localparam key_ctrl 		= 8'h14;
localparam key_caps 	= 8'h58;
localparam key_break 	= 8'hf0;


always@(*) begin
	case(data_in)
		key_0	:	data_out = asci_0 ;
		key_1	:	data_out = asci_1 ;
		key_2	:	data_out = asci_2 ;
		key_3	:	data_out = asci_3 ;
		key_4	:	data_out = asci_4 ;
		key_5	:	data_out = asci_5 ;
		key_6	:	data_out = asci_6 ;
		key_7	:	data_out = asci_7 ;
        key_8	:	data_out = asci_8 ;
        key_9	:	data_out = asci_9 ;
        key_a	:	if(uppercase) data_out = asci_A; else data_out = asci_a 	;
        key_b	:	if(uppercase) data_out = asci_B; else data_out = asci_b 	;
        key_c	:	if(uppercase) data_out = asci_C; else data_out = asci_c 	;
        key_d	:	if(uppercase) data_out = asci_D; else data_out = asci_d 	;
        key_e	:	if(uppercase) data_out = asci_E; else data_out = asci_e 	;
        key_f 	:	if(uppercase) data_out = asci_F; else data_out = asci_f  	;
        key_g	:	if(uppercase) data_out = asci_G; else data_out = asci_g 	;
        key_h	:	if(uppercase) data_out = asci_H; else data_out = asci_h 	;
        key_i 	:	if(uppercase) data_out = asci_I ; else data_out = asci_i  	;
        key_j 	:	if(uppercase) data_out = asci_J; else data_out = asci_j  	;
        key_k	:	if(uppercase) data_out = asci_K; else data_out = asci_k 	;
        key_l 	:	if(uppercase) data_out = asci_L; else data_out = asci_l  	;
        key_m	:	if(uppercase) data_out = asci_M; else data_out = asci_m	;
        key_n	:	if(uppercase) data_out = asci_N; else data_out = asci_n 	;
        key_o	:	if(uppercase) data_out = asci_O; else data_out = asci_o 	;
        key_p	:	if(uppercase) data_out = asci_P; else data_out = asci_p 	;
        key_q	:	if(uppercase) data_out = asci_Q; else data_out = asci_q 	;
        key_r 	:	if(uppercase) data_out = asci_R; else data_out = asci_r  	;
        key_s	:	if(uppercase) data_out = asci_S; else data_out = asci_s 	;
        key_t 	:	if(uppercase) data_out = asci_T; else data_out = asci_t  	;
        key_u	:	if(uppercase) data_out = asci_U; else data_out = asci_u 	;
        key_v	:	if(uppercase) data_out = asci_V; else data_out = asci_v 	;
        key_w	:	if(uppercase) data_out = asci_W; else data_out = asci_w	;
        key_x	:	if(uppercase) data_out = asci_X; else data_out = asci_x 	;
        key_y	:	if(uppercase) data_out = asci_Y; else data_out = asci_y 	;
        key_z	:	if(uppercase) data_out = asci_Z; else data_out = asci_z 	;
		
		key_shift 		: data_out = data_in;
		key_ctrl		: data_out = data_in;
		key_caps		: data_out = data_in;
		key_break	: data_out = data_in;
		default :	data_out = 8'h0;
	endcase
end

endmodule