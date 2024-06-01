CPP=g++

VERBOSE=0
GENERAL-FLAGS=-Wall -Wextra -Werror -Wswitch-enum -std=c++17 -I$(shell pwd) -I$(shell pwd)/avm
DEBUG-FLAGS=-ggdb -fsanitize=address
RELEASE-FLAGS=-O3
CPPFLAGS:=$(GENERAL-FLAGS) -std=c++17 $(DEBUG-FLAGS) -D VERBOSE=$(VERBOSE)

LIBS=-lm
DIST=build
TERM_YELLOW:=$(shell echo -e "\e[0;33m")
TERM_GREEN:=$(shell echo -e "\e[0;32m")
TERM_RESET:=$(shell echo -e "\e[0;0m")

# Setup variables for source code, output, etc
## ASSEMBLY setup
SRC=src
CODE:=$(addprefix $(SRC)/, base.cpp lexer.cpp)
OBJECTS:=$(CODE:$(SRC)/%.cpp=$(DIST)/%.o)
OUT=$(DIST)/asm.out

## EXAMPLES setup
EXAMPLES_DIST=$(DIST)/examples
EXAMPLES_SRC=examples
EXAMPLES=$(EXAMPLES_DIST)/instruction-test.out $(EXAMPLES_DIST)/fib.out $(EXAMPLES_DIST)/factorial.out $(EXAMPLES_DIST)/memory-print.out

## Dependencies
DEPDIR:=$(DIST)/dependencies
DEPFLAGS = -MT $@ -MMD -MP -MF
DEPS:=$(CODE:$(SRC)/%.cpp=$(DEPDIR)/%.o)

## AVM
AVM_ROOT_DIR=avm
AVM_DIR=$(AVM_ROOT_DIR)/lib
AVM_DIST=$(AVM_ROOT_DIR)/build/lib
AVM_SOURCE:=$(shell find $(AVM_DIR) -name '*.c')
AVM_OBJECTS:=$(AVM_SOURCE:$(AVM_DIR)/%.c=$(AVM_DIST)/%.o)
VM_OUT:=$(AVM_ROOT_DIR)/build/avm.out

# Things you want to build on `make`
all: $(DIST) asm examples

asm: $(OUT)
examples: $(EXAMPLES)

# Recipes
## ASSEMBLY Recipes
$(OUT): $(AVM_OBJECTS) $(OBJECTS) $(DIST)/main.o
	@$(CPP) $(CPPFLAGS) $^ -o $@ $(LIBS)
	@echo "$(TERM_GREEN)$@$(TERM_RESET): $^"

$(DIST)/%.o: $(SRC)/%.cpp | $(DIST) $(DEPDIR)/asm
	@$(CPP) $(CPPFLAGS) $(DEPFLAGS) $(DEPDIR)/asm/$*.d -c $< -o $@ $(LIBS)
	@echo "$(TERM_YELLOW)$@$(TERM_RESET): $<"

## EXAMPLES recipes
$(EXAMPLES_DIST)/%.out: $(EXAMPLES_SRC)/%.asm $(OUT) | $(EXAMPLES_DIST)
	@$(OUT) $< $@
	@echo "$(TERM_GREEN)$@$(TERM_RESET): $<"

.PHONY: run-examples
run-examples: $(EXAMPLES)
	@$(foreach example,$(EXAMPLES), echo "$(TERM_YELLOW)$(example)$(TERM_RESET)"; $(MAKE) -s interpret BYTECODE=$(example);)

## libavm
$(AVM_OBJECTS): libavm;
$(VM_OUT): avm.exe;

.PHONY: libavm
libavm:
	@$(MAKE) -C $(AVM_ROOT_DIR) lib

.PHONY: avm.exe
avm.exe:
	@$(MAKE) -C $(AVM_ROOT_DIR) vm

ARGS=

.PHONY: run
run: $(OUT)
	./$^ $(ARGS)

.PHONY: clean
clean:
	@rm -rfv $(DIST)/*
	@$(MAKE) -C $(AVM_ROOT_DIR) clean

SOURCE=
BYTECODE=
.PHONY: assemble
assemble: $(OUT)
	@$(OUT) $(SOURCE) $(BYTECODE)

.PHONY: interpret
interpret: $(VM_OUT)
	@$(VM_OUT) $(BYTECODE)

.PHONY: exec
exec: $(OUT) $(VM_OUT)
	@$(OUT) $(SOURCE) $(BYTECODE)
	@$(VM_OUT) $(BYTECODE)

# Directories
$(DIST):
	@mkdir -p $@

$(EXAMPLES_DIST):
	@mkdir -p $@

$(DEPDIR)/asm:
	@mkdir -p $@

-include $(wildcard $(DEPS))
