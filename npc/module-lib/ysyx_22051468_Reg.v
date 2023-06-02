localparam REG_WIDTH = 5;
localparam REG_NUM = 32;

module ysyx_22051468_Reg #(
    WIDTH = 64
)
(
    input clk,
    input rst_n,
    // from Inst_decode
    input [REG_WIDTH-1:0] rs1_raddr_i,
    input [REG_WIDTH-1:0] rs2_raddr_i,
    // to Inst_decode_dff
    output reg [WIDTH-1:0] rs1_data,
    output reg [WIDTH-1:0] rs2_data,
    // from exec
    input [REG_WIDTH-1:0] rd_waddr_i,
    input [WIDTH-1:0] rd_wdata_i,
    input wen,

// to top
    output [WIDTH*REG_NUM-1:0] GPR2TOP
);

// define 32 gprs, each has 64 bits for RV64
reg [WIDTH-1:0] gpr [REG_NUM-1:0];


// if rd_waddr = 0 and wen = 1, then we just set en =0;
wire write_en;
assign write_en = (rd_waddr_i == 0) ? 0 : wen;



// in case of data collidation
always @(posedge clk) begin
    if(!rst_n) begin
        for(integer i=0;i<32;i=i+1) begin
            gpr[i] <= 64'b0;
        end
    end else begin
        if(write_en) begin
            gpr[rd_waddr_i] <= rd_wdata_i;
        end
    end
end

always@(*) begin
    if(write_en && (rd_waddr_i == rs1_raddr_i)) begin
        rs1_data = rd_wdata_i;
    end else begin
        rs1_data = gpr[rs1_raddr_i];
    end

end

always@(*) begin
    if(write_en && (rd_waddr_i == rs2_raddr_i)) begin
        rs2_data = rd_wdata_i;
    end else begin
        rs2_data = gpr[rs2_raddr_i];
    end

end


endmodule