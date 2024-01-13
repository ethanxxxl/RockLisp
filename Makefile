# Project: Rock Lisp

# Programmer: Ethan Smith

# Purpose: A simple lisp compiler written in C

# Description: This is the project makefile. It manages building both the
# "chisel" compiler and rocklisp targets. All dependencies are handled, so
# simply calling "make {rocklisp target}" will compile the compiler (if
# necessary), and then compile the rocklisp file. The binary locations are
# configurable through the BIN_DIR variable. For convenience they can be set to
# the root directory. "make clean" can detect this and will remove all
# appropriate build file/directories, without deleting "./".

# OUTPUT DIRECOTIES
BIN_DIR = .
BUILD_DIR = .build

#
# C TOOL CHAIN FOR COMPILING THE COMPILER
#
CFLAGS = -Werror -Wall -Wextra -Wpedantic -std=gnu17 -g
LIBS = -lm
CC = clang

COMPILER_SRC = src/chisel.c src/parser.c src/code-generator.c src/scanner.c
COMPILER_INC = -Iinclude/

COMPILER_OBJ = $(patsubst src/%.c,$(BUILD_DIR)/chisel/%.o,$(COMPILER_SRC))
CHISEL = $(BIN_DIR)/chisel

#
# ROCKLISP TOOLCHAIN
#
ROCKLISP_DIR = rocklisp

ROCKLISP_SRC = $(wildcard $(ROCKLISP_DIR)/*.rl)
ROCKLISP_ASM = $(patsubst %.rl,$(BUILD_DIR)/%.s,$(ROCKLISP_SRC))
ROCKLISP_OBJS = $(patsubst %.s,%.o,$(ROCKLISP_ASM))
ROCKLISP_TGTS = $(patsubst $(ROCKLISP_DIR)/%.rl,$(BIN_DIR)/%,$(ROCKLISP_SRC))

all: $(CHISEL) $(ROCKLISP_TGTS)

# CHISEL COMPILER TARGET
$(CHISEL): $(COMPILER_OBJ)
	@mkdir -p $(@D)
	@echo -e "\033[1mcompiling $@\033[0m"
	@$(CC) $(CFLAGS) $(COMPILER_INC) $(LIB) $^ -o $@

$(COMPILER_OBJ): $(BUILD_DIR)/chisel/%.o : src/%.c
	@mkdir -p $(@D)
	@echo -e "\033[3mcompiling $<\033[0m"
	@$(CC) $(CFLAGS) $(COMPILER_INC) $(LIB) -c $< -o $@

# INDIVIDUAL ROCKLISP TARGETS
$(ROCKLISP_TGTS): $(BIN_DIR)/% : $(BUILD_DIR)/$(ROCKLISP_DIR)/%.o
	@mkdir -p $(@D)
	@echo TEST RUN: ld $< -o $@

# ASSEMBLE ROCKLISP OBJECT FILES
$(ROCKLISP_OBJS): %.o : %.s
	@mkdir -p $(@D)
	@echo TEST RUN: as $< -o $@

# COMPILE ROCKLISP SOURCE INTO x86 ASSEMBLY
$(ROCKLISP_ASM): $(BUILD_DIR)/%.s : %.rl chisel
	@mkdir -p $(@D)
	@echo TEST RUN: ./chisel $< -o $@

.PHONY: clean
clean:
        # remove build files, taking caution not to delete the . directory
	@if [[ $(BUILD_DIR) -ef . ]]; then \
	    echo -e "\033[31mremoving:\033[0m"\
                    $(ROCKLISP_OBJS) $(ROCKLISP_ASM) $(COMPILER_OBJ); \
	    rm -f $(ROCKLISP_OBJS) $(ROCKLISP_ASM) $(COMPILER_OBJ); \
	else \
	    echo -e "\033[31mremoving:\033[0m" $(BUILD_DIR); \
	    rm -rf $(BUILD_DIR); \
	fi

        # remove binaries, takning caution not to delete the . direcoty
	@if [[ $(BIN_DIR) -ef . ]]; then \
	    echo -e "\033[31mremoving:\033[0m" $(CHISEL) $(ROCKLISP_TGTS); \
	    rm -f $(CHISEL) $(ROCKLISP_TGTS); \
        else \
	    echo -e "\033[31mremoving:\033[0m" $(BIN_DIR); \
	    rm -rf $(BIN_DIR); \
        fi
