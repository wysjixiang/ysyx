
TOPNAME := top
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

VSRC_FILE := vsrc
CSRC_FILE := csrc

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

VSRCS := $(shell find $(abspath ./$(VSRC_FILE)) -name "*.v") 
CSRCS := $(shell find $(abspath ./$(CSRC_FILE)) -name "*.c" -or -name "*.cpp" -or -name "*.cc")

include ../common.mk
