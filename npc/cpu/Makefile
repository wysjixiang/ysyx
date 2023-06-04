
WORK_DIR:= \
$(abspath $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))))
TOPNAME := top
NXDC_FILES := $(WORK_DIR)/constr/top.nxdc
#INC_PATH := $(NPC_HOME)/module-lib 

VSRC_FILE := module-lib

BUILD_DIR := $(WORK_DIR)/build
OBJ_DIR := $(BUILD_DIR)/obj_dir
BIN := $(BUILD_DIR)/$(TOPNAME)

VSRCS := $(WORK_DIR)/top.v
CSRCS := $(shell find $(WORK_DIR) -name "*.c" -or -name "*.cpp" -or -name "*.cc")

VSRCS += $(shell find $(NPC_HOME)/$(VSRC_FILE) -name "*.v") 


include $(NPC_HOME)/common.mk


#VSRCS += $(shell find $(abspath ../$(VSRC_FILE)/MUX) -name "*.v") 
#VSRCS += $(shell find $(abspath ../$(VSRC_FILE)/NVBoard) -name "*.v") 
