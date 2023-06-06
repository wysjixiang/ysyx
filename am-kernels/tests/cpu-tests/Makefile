.PHONY: all run gdb clean latest $(ALL)

# adding default ARCH
ARCH ?=riscv64-npc

ifeq ($(ARCH),riscv64-npc)
NPCFLAGS := -b
## passing elf file to main function
#NEMUFLAGS += -f $(IMAGE).elf
endif

RESULT = .result
$(shell > $(RESULT))

COLOR_RED   = \033[1;31m
COLOR_GREEN = \033[1;32m
COLOR_NONE  = \033[0m

ALL ?= $(basename $(notdir $(shell find tests/. -name "*.c")))

all: $(addprefix Makefile., $(ALL))
	@echo "" $(ALL)

$(ALL): %: Makefile.%

Makefile.%: tests/%.c latest
	#modified a little bit here!
	@/bin/echo -e "NPCFLAGS += $(NPCFLAGS)\nNAME = $*\nSRCS = $<\nLIBS += klib\ninclude $${AM_HOME}/Makefile" > $@
	@if make -s -f $@ ARCH=$(ARCH) $(MAKECMDGOALS); then \
		printf "[%14s] $(COLOR_GREEN)PASS!$(COLOR_NONE)\n" $* >> $(RESULT); \
	else \
		printf "[%14s] $(COLOR_RED)FAIL!$(COLOR_NONE)\n" $* >> $(RESULT); \
	fi
	-@rm -f Makefile.$*

run: all
	@cat $(RESULT)
	@rm $(RESULT)

gdb: all

ifeq ($(ARCH), riscv64-npc)
clean:
	rm -rf Makefile.* build/ $(NPC_HOME)/cpu/build/
else
clean:
	rm -rf Makefile.* build/
endif

latest:
