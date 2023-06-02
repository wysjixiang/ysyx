`include "INST_TYPE.v"

module ysyx_22051468_Rs_Imm_Need(
    input [3:0] opcode_tag,
    output rd_need,
    output rs1_need,
    output rs2_need,
    output imm_need
);

assign rd_need = 
    opcode_tag == `INST_U_AUIPC | 
    opcode_tag == `INST_U_LUI | 
    opcode_tag == `INST_I_ | 
    opcode_tag == `INST_I_LOAD | 
    opcode_tag == `INST_I_W    | 
    opcode_tag == `INST_I_JALR| 
    opcode_tag == `INST_J_JAL| 
    opcode_tag ==  `INST_R_   |
    opcode_tag ==  `INST_R_W    
    ;

assign rs1_need = 
    opcode_tag == `INST_I_ | 
    opcode_tag == `INST_I_LOAD | 
    opcode_tag == `INST_I_W    | 
    opcode_tag == `INST_I_JALR| 
    opcode_tag ==  `INST_R_   |
    opcode_tag ==  `INST_R_W  |  
    opcode_tag ==  `INST_B_   |   
    opcode_tag ==  `INST_S_      
    ;


assign rs2_need = 
    opcode_tag ==  `INST_R_   |
    opcode_tag ==  `INST_R_W  |  
    opcode_tag ==  `INST_B_   |   
    opcode_tag ==  `INST_S_      
    ;


assign imm_need = 
    opcode_tag == `INST_U_AUIPC | 
    opcode_tag == `INST_U_LUI | 
    opcode_tag == `INST_I_ | 
    opcode_tag == `INST_I_LOAD | 
    opcode_tag == `INST_I_W    | 
    opcode_tag == `INST_I_JALR| 
    opcode_tag == `INST_J_JAL| 
    opcode_tag ==  `INST_R_   |
    opcode_tag ==  `INST_R_W  |  
    opcode_tag ==  `INST_B_   |   
    opcode_tag ==  `INST_S_      
    ;


endmodule
