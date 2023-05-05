TOPNAME = top
NXDC_FILES = constr/top.nxdc
INC_PATH ?=

VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc  \
				-O3 --x-assign fast --x-initial fast --noassert
# C++ compiler arguments for makefile .check via man
# CFLAGS<flags>:Verilator compiler arguments for makefile
# -MMD: create phony dependency targets
# --build: build model executable/library after verilation
# -cc: create c++ output
# -o3: optimization option- high-performance optimizations
# --x-assign fast: --x-assign <mode> -> assign non-initial Xs to this value
# --x-initial fast: similart to above, assign initial Xs to this value
# --noassert: disable assertion

BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

default: $(BIN)

$(shell mkdir -p $(BUILD_DIR))
# -p: no error if $(BIULD_DIR) existing, else make directories as needed(make a directory if not exists)

# constraint file
SRC_AUTO_BIND := $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@
# this rule generate auto_bind.cpp file to bind the pins to nvboard via python code.

# project source
VSRCS := $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS := $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")
# find is not a makefile funcion, so we must use shell command to call it. find -name is to find the relavent file in certain path. With -or argument to add choices,

CSRCS += $(SRC_AUTO_BIND)
# add auto_bind.cpp file to CSRCS

# rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
# -I: directory to search for includes
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
# -D<Var>[=<value>] : Set preprocessor define
LDFLAGS += -lSDL2 -lSDL2_image
# -LDFLAGS : linker pre-object arguments for makefile 

$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))
# --top-module : assign the name of top-level input module
# --Mdir : assign the name of output object directory
# abspath : return abspath for $(BIN)

all: default

run: $(BIN)
	@$^
# run bin file

clean:
	rm -rf $(BUILD_DIR)

.PHONY: default all clean run
