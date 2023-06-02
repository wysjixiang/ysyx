`include "INST_TYPE.v"

module ysyx_22051468_InstDecode #(
    WIDTH = 64
)
(
    // from Ifetch_Dff
    input [`INST_WIDTH-1:0] inst_i,
    input [WIDTH-1:0] inst_addr_i,
    // to regs
    output [4:0] rs1_addr_o,
    output [4:0] rs2_addr_o,
    // to Dff
    output [`INST_WIDTH-1:0] inst_o,
    output [WIDTH-1:0] inst_addr_o,
    // gpr & imm related
    output [WIDTH-1:0] imm_o,
    output [4:0] rd_addr_o,
    // need sig
    output rd_need,
    output rs1_need,
    output rs2_need,
    output imm_need,
    // Bus related!
    // need to inform bus one cycle ahead
    output is_load,
    output is_store,
    // inst decode info
    output [3:0] inst_type_o,
    output is_branch,
    output is_jal,
    output is_jalr,
    output is_U,
    output is_W,
    output is_mul,
    output is_div,
    output is_rem,
    // define the explicit_type with one hot coding
    output [`EXPLICIT_TYPE_NUM-1:0] explicit_type_o
);

wire [6:0] opcode;
wire [2:0] funct3;
wire [6:0] funct7;
wire [WIDTH-1:0] immI;
wire [WIDTH-1:0] immS;
wire [WIDTH-1:0] immB;
wire [WIDTH-1:0] immU;
wire [WIDTH-1:0] immJ;


// imm_sel
wire imm_i_sel;
wire imm_s_sel;
wire imm_b_sel;
wire imm_u_sel;
wire imm_j_sel;

// LUT related
`define OP_LUT_NRKEY 12
`define OP_LUT_KEYLEN 7
`define OP_LUT_DATALEN 4
wire [(`OP_LUT_KEYLEN + `OP_LUT_DATALEN) * `OP_LUT_NRKEY-1 : 0] OPCODE_LUT;
assign OPCODE_LUT = {
`TYPE_U_AUIPC  ,    `INST_U_AUIPC, 
`TYPE_U_LUI    ,    `INST_U_LUI  ,   
`TYPE_I_       ,    `INST_I_     ,   
`TYPE_I_LOAD   ,    `INST_I_LOAD ,   
`TYPE_I_W      ,    `INST_I_W    ,   
`TYPE_I_JALR   ,    `INST_I_JALR ,   
`TYPE_J_JAL    ,    `INST_J_JAL  ,   
`TYPE_R_       ,    `INST_R_     ,   
`TYPE_R_W      ,    `INST_R_W    ,   
`TYPE_B_       ,    `INST_B_     ,   
`TYPE_S_       ,    `INST_S_     ,   
`TYPE_EBREAK   ,    `INST_EBREAK  
};
wire [3:0] OPCODE_TYPE;

// assign 
assign opcode = inst_i[6:0];
assign funct3 = inst_i[14:12];
assign funct7 = inst_i[31:25];
assign immI = {{(WIDTH-12){inst_i[31]}},inst_i[31:20]};
assign immS = {{(WIDTH-12){inst_i[31]}},inst_i[31:25],inst_i[11:7] };
assign immB = {
    {(WIDTH-13){inst_i[31]}},
    inst_i[31],
    inst_i[7],
    inst_i[30:25],
    inst_i[11:8],
    1'b0
};
assign immU = {
    {(WIDTH-21){inst_i[31]}},
    inst_i[31],
    inst_i[19:12],
    inst_i[20],
    inst_i[30:25],
    inst_i[24:21],
    1'b0
};

// assign imm related
assign imm_o = 
    ({WIDTH{imm_i_sel}} & immI) |
    ({WIDTH{imm_s_sel}} & immS) |
    ({WIDTH{imm_b_sel}} & immB) |
    ({WIDTH{imm_u_sel}} & immU) |
    ({WIDTH{imm_j_sel}} & immJ) 
    ;

// assign imm_sel
assign imm_i_sel = 
    (OPCODE_TYPE == `INST_I_    )|
    (OPCODE_TYPE == `INST_I_LOAD)|
    (OPCODE_TYPE == `INST_I_W   )|
    (OPCODE_TYPE == `INST_I_JALR)
    ;

assign imm_s_sel = 
    (OPCODE_TYPE == `INST_S_)
    ;

assign imm_b_sel = 
    (OPCODE_TYPE == `INST_B_)
    ;

assign imm_u_sel = 
    (OPCODE_TYPE == `INST_U_AUIPC)|
    (OPCODE_TYPE == `INST_U_LUI  )
    ;

assign imm_j_sel = 
    (OPCODE_TYPE == `INST_J_JAL)
    ;

// assign output
assign rs1_addr_o = inst_i[19:15];
assign rs2_addr_o = inst_i[24:20];
assign rd_addr_o = inst_i[11:7];
assign inst_o = inst_i;
assign inst_addr_o = inst_addr_i;


ysyx_22051468_Rs_Imm_Need RS0(
    .opcode_tag (OPCODE_TYPE) ,
    .rd_need    (rd_need)    ,
    .rs1_need   (rs1_need) ,
    .rs2_need   (rs2_need) ,
    .imm_need   (imm_need)     
);


ysyx_22051468_Opcode_Decode  OPCODE0(
    .op_i            (OPCODE_TYPE) ,
    .funct3          (funct3) ,
    .funct7          (funct7) ,
    .is_load         (is_load    ),
    .is_store        (is_store   ),
    .inst_type_o     (inst_type_o),
    .is_branch       (is_branch  ),
    .is_jal          (is_jal     ),
    .is_jalr         (is_jalr    ),
    .is_U            (is_U       ),
    .is_W            (is_W       ),
    .is_mul          (is_mul),
    .is_div          (is_div),
    .is_rem          (is_rem),
    .explicit_type_o (explicit_type_o) 
);

// first handle the opcode to find the inst type roughly
ysyx_22051468_MuxKeyNoDefault #(
    .NR_KEY(`OP_LUT_NRKEY),
    .KEY_LEN(`OP_LUT_KEYLEN),
    .DATA_LEN(`OP_LUT_DATALEN)
)   MUX_0(
    .key(opcode),
    .lut(OPCODE_LUT),
    .out(OPCODE_TYPE)
);




endmodule