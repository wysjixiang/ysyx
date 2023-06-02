/*
`include "INST_TYPE.v"

/* opcode lut
`define OPCODE_LUT {
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
}

`define OP_LUT_NRKEY 12
`define OP_LUT_KEYLEN 7
`define OP_LUT_DATALEN 4

module ysyx_22051468_OPCODE_LUT_INIT(
   output [(`OP_LUT_KEYLEN + `OP_LUT_DATALEN) * `OP_LUT_NRKEY-1 : 0] OPCODE_LUT
);

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

endmodule


*/