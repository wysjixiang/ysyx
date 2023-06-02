


module ysyx_22051468_Shift_64  #(
    WIDTH = 64
)
(
    input [WIDTH-1:0] data_i,
    input [5:0] shamt,
    input LorR,
    input AorL,
    output [WIDTH-1:0] data_o
);

wire high_bit;

localparam N1 = 1;
localparam N2 = 2;
localparam N3 = 4;
localparam N4 = 8;
localparam N5 = 16;
localparam N6 = 32;
wire [WIDTH-1:0] level1_data_out;
wire [WIDTH-1:0] level2_data_out;
wire [WIDTH-1:0] level3_data_out;
wire [WIDTH-1:0] level4_data_out;
wire [WIDTH-1:0] level5_data_out;
wire [WIDTH-1:0] level6_data_out;
wire level1_shamt;
wire level2_shamt;
wire level3_shamt;
wire level4_shamt;
wire level5_shamt;
wire level6_shamt;

// assign
assign high_bit = AorL & data_i[WIDTH-1];
assign level1_shamt   = shamt[0];
assign level2_shamt   = shamt[1];
assign level3_shamt   = shamt[2];
assign level4_shamt   = shamt[3];
assign level5_shamt   = shamt[4];
assign level6_shamt   = shamt[5];
assign data_o = level6_data_out;

// first level shift -> 1
ysyx_22051468_BarrelShiftLayer   #(
    .WIDTH(64),
    .N(N1)
)   Layer1(
    .data_i    (data_i) ,
    .shamt     (level1_shamt) ,
    .LorR      (LorR) ,
    .high_bit  (high_bit) ,
    .data_out  (level1_data_out) 
);

// first level shift -> 2
ysyx_22051468_BarrelShiftLayer   #(
    .WIDTH(64),
    .N(N2)
)   Layer2(
    .data_i    (level1_data_out) ,
    .shamt     (level2_shamt) ,
    .LorR      (LorR) ,
    .high_bit  (high_bit) ,
    .data_out  (level2_data_out) 
);


// first level shift -> 4
ysyx_22051468_BarrelShiftLayer   #(
    .WIDTH(64),
    .N(N3)
)   Layer3(
    .data_i    (level2_data_out) ,
    .shamt     (level3_shamt) ,
    .LorR      (LorR) ,
    .high_bit  (high_bit) ,
    .data_out  (level3_data_out) 
);

// first level shift -> 8
ysyx_22051468_BarrelShiftLayer   #(
    .WIDTH(64),
    .N(N4)
)   Layer4(
    .data_i    (level3_data_out) ,
    .shamt     (level4_shamt) ,
    .LorR      (LorR) ,
    .high_bit  (high_bit) ,
    .data_out  (level4_data_out) 
);

// first level shift -> 16
ysyx_22051468_BarrelShiftLayer   #(
    .WIDTH(64),
    .N(N5)
)   Layer5(
    .data_i    (level4_data_out) ,
    .shamt     (level5_shamt) ,
    .LorR      (LorR) ,
    .high_bit  (high_bit) ,
    .data_out  (level5_data_out) 
);

// first level shift -> 32
ysyx_22051468_BarrelShiftLayer   #(
    .WIDTH(64),
    .N(N6)
)   Layer6(
    .data_i    (level5_data_out) ,
    .shamt     (level5_shamt) ,
    .LorR      (LorR) ,
    .high_bit  (high_bit) ,
    .data_out  (level6_data_out) 
);


endmodule