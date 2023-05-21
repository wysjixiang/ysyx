
TOPNAME := top
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

VSRC_FILE := module-lib

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

VSRCS := ./top.v
CSRCS := $(shell find $(abspath ./) -name "*.c" -or -name "*.cpp" -or -name "*.cc")

VSRCS += $(shell find $(abspath ../$(VSRC_FILE)) -name "*.v") 

include ../common.mk
