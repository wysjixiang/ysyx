Vfile := ./vsrc/
Cfile := ./csrc/

SRC_C := tb_alu
SRC_V := alu


.PHONY: default
default:
	@echo "------- Invalid command ---------"
	@echo "v------Compile .v/.sv file to c++"
	@echo "run----After -v, run this--------"
	@echo "wave---Open GTKwave--------------"
	@echo "---------------------------------"


.PHONY: v
v:	$(Vfile)$(SRC_V).v
	@echo "------ Generate obj_dir ---------"
	verilator --cc	$(Vfile)$(SRC_V).v

.PHONY: run
#run:	./obj_dir	V$(SRC_V).mk	V$(SRC_V)
run:	
	@echo "------ Verilate & Build ---------"
	verilator -Wall --trace --cc $(Vfile)$(SRC_V).v --exe $(Cfile)$(SRC_C).cpp 
	make -C obj_dir -f V$(SRC_V).mk V$(SRC_V)
	./obj_dir/V$(SRC_V)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "------ Complete! ----------------"

.PHONY:	wave
wave:	waveform.vcd
	@echo "------ Open GTKwaveform ---------"
	gtkwave	waveform.vcd

all:
	@echo "Write this Makefile by your self."

sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."


.PHONY: clean
clean:
	@echo "---------clean---------"
	rm -rf obj_dir 


include ../Makefile
