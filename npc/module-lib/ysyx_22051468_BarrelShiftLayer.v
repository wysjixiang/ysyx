
module ysyx_22051468_BarrelShiftLayer   #(
    WIDTH = 64,
    N = 1
)
(
    input [WIDTH-1:0] data_i,
    input shamt,
    input LorR,
    input high_bit,
    output [WIDTH-1:0] data_out
);
// level shift -> N
genvar i;
generate

// low
    for(i=0;i<N;i=i+1) begin
        ysyx_22051468_MuxKeyWithDefault #(2,2,1) loop1_low_mux1(
        .out(data_out[i])        ,
        .key({LorR,shamt})   ,
        .default_out(data_i[i])     ,
        .lut(
            {
                2'b01,data_i[i+N],
                2'b11,1'b0
            }
            )
        );
    end
// mid
    for(i=N;i<WIDTH-N;i=i+1) begin
        ysyx_22051468_MuxKeyWithDefault #(2,2,1) loop1_mid_mux1(
        .out(data_out[i])        ,
        .key({LorR,shamt})   ,
        .default_out(data_i[i])     ,
        .lut(
            {
                2'b01,data_i[i+N],
                2'b11,data_i[i-N]
            }
            )
        );
    end
// upper
    for(i=WIDTH-N;i<WIDTH;i=i+1) begin
        ysyx_22051468_MuxKeyWithDefault #(2,2,1) loop1_upper_mux1(
        .out(data_out[i])        ,
        .key({LorR,shamt})   ,
        .default_out(data_i[i])     ,
        .lut(
            {
                2'b01,high_bit,
                2'b11,data_i[i-N]
            }
            )
        );
    end
endgenerate




endmodule