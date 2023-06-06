
VERILATOR := verilator
VERILATOR_CFLAGS += \
	-MMD --build -cc -trace\
	-O3 --x-assign fast --x-initial fast --noassert


.PHONY: default
default: 
	@echo "------- Invalid command ---------" 
	@echo "run----Run based on Nvboard------" 
	@echo "wave---Open GTKwave--------------" 
	@echo "---------------------------------"

$(shell mkdir -p $(BUILD_DIR))

SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)

$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@


CSRCS += $(SRC_AUTO_BIND)

#rules for NVBoard
include $(NVBOARD_HOME)/scripts/nvboard.mk
# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
# -I: directory to search for includes
CFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""
# -D<Var>[=<value>] : Set preprocessor define
LDFLAGS += -lSDL2 -lSDL2_image
# add readline lib for read input and parse
LDFLAGS += -lreadline 
# -LDFLAGS : linker pre-object arguments for makefile 

# add llvm
LDFLAGS += $(shell llvm-config --cxxflags) -fPIE
LDFLAGS += $(shell llvm-config --libs) 

#add REF_SO
LDFLAGS += -I$(NEMU_HOME)/build
LDFLAGS += $(NEMU_HOME)/build/riscv64-nemu-interpreter-so


$(BIN): $(VSRCS) $(CSRCS) $(NVBOARD_ARCHIVE) 
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
	-I$(NPC_HOME)/module-lib \
	--top-module $(TOPNAME) $^ \
	$(addprefix -CFLAGS , $(CFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
	--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))
	$(call git_commit, "Run bin file") # DO NOT REMOVE THIS LINE!!!

IMG ?=

NPC_EXEC := $(BIN) $(IMG)

run: $(BIN)
	$(NPC_EXEC)

clean: 
	rm -rf $(BUILD_DIR) $(wildcard *.vcd)


.PHONY: run clean wave


.PHONY:	wave
wave:	run 
	@echo "------ Open GTKwaveform ---------"
	gtkwave	waveform.vcd



include $(NEMU_HOME)/../Makefile

# include for tracer-ysyx
