FASTA ?=
LCP_LEVEL ?= 4

# Programs
LCP_FASTA := lcp-fasta
LCP_FASTA_MEM := lcp-fasta-mem
MINIMIZER_FASTA := minimizer-fasta
SYNCMER_FASTA := syncmer-fasta

# Directories
CURRENT_DIR := $(shell pwd)
DATA_DIR := $(CURRENT_DIR)/../data
EXECUTABLE_DIR := $(CURRENT_DIR)/../bin
OUT_DIR := $(CURRENT_DIR)/../out
INCLUDE_DIR ?= $(CURRENT_DIR)/../include
LIB_DIR ?= $(CURRENT_DIR)/../lib

# Extension
CXX := .cpp

# Compiler
GXX := g++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra
TIME := /usr/bin/time -v

.PHONY: all

all: mkdir_bin mkdir_out

mkdir_bin:
	@echo "Checking if $(EXECUTABLE_DIR) exists..."
	@if [ ! -d "$(EXECUTABLE_DIR)" ]; then \
		echo "$(EXECUTABLE_DIR) does not exist. Creating..."; \
		mkdir $(EXECUTABLE_DIR); \
	else \
		echo "$(EXECUTABLE_DIR) already exists."; \
	fi

mkdir_out:
	@echo "Checking if $(OUT_DIR) exists..."
	@if [ ! -d "$(OUT_DIR)" ]; then \
		echo "$(OUT_DIR) does not exist. Creating..."; \
		mkdir $(OUT_DIR); \
	else \
		echo "$(OUT_DIR) already exists."; \
	fi

check_lcptoolsS:
	@if [ ! -d "$(INCLUDE_DIR)" ]; then \
		echo "$(INCLUDE_DIR) does not exist. Please compile lcptoolsS to headers to $(INCLUDE_DIR)"; \
		exit 1; \
	fi
	@if [ ! -d "$(LIB_DIR)" ]; then \
		echo "$(LIB_DIR) does not exist. Please compile lcptoolsS and put library to $(LIB_DIR)"; \
		exit 1; \
	fi

check_fasta:
	@if [ -z "$(FASTA)" ]; then \
        echo "FASTA is not set or is empty. Exiting"; \
		exit 1; \
    fi
	@if [ ! -f $(FASTA) ]; then \
		echo "$(FASTA) does not exist. Exiting."; \
		exit 1; \
	fi

install:
	@if [ ! -f $(DATA_DIR)/$(FASTA) ]; then \
		wget http://hgdownload.soe.ucsc.edu/goldenPath/hg38/bigZips/hg38.fa.gz -P $(DATA_DIR)/; \
		gunzip $(DATA_DIR)/hg38.fa.gz; \
	fi

clean:
	@if [ -d "$(OUT_DIR)" ]; then \
		echo "Removing $(OUT_DIR)"; \
		rm -r $(OUT_DIR); \
	fi
	@if [ -d "$(EXECUTABLE_DIR)" ]; then \
		echo "Removing $(EXECUTABLE_DIR)"; \
		rm -r $(EXECUTABLE_DIR); \
	fi
	@echo "Cleaned."

######################################################################
# LCP
######################################################################
lcpFasta: mkdir_bin check_fasta check_lcptoolsS
	@echo "Compiling $(LCP_FASTA)$(CXX)"
	$(GXX) $(CXXFLAGS) -DSTATS -I$(INCLUDE_DIR) -c $(LCP_FASTA)$(CXX) -o $(LCP_FASTA).o
	@echo "Linking lcptoolsS library and binary files"
	$(GXX) $(CXXFLAGS) -o $(LCP_FASTA) $(LCP_FASTA).o -L$(LIB_DIR) -llcptoolsS -Wl,-rpath,$(LIB_DIR)
	rm $(LCP_FASTA).o
	@echo "Moving $(LCP_FASTA) to $(EXECUTABLE_DIR)"
	mv $(LCP_FASTA) $(EXECUTABLE_DIR)
	@echo "Preprocessing $(FASTA). Output will be put into $(OUT_DIR)/$(LCP_FASTA)-output.txt"
	$(TIME) $(EXECUTABLE_DIR)/$(LCP_FASTA) $(FASTA) > $(OUT_DIR)/$(LCP_FASTA)-output.txt 2>&1

lcpFastaMem: mkdir_bin check_fasta check_lcptoolsS
	@echo "Compiling $(LCP_FASTA_MEM)$(CXX)"
	$(GXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $(LCP_FASTA_MEM)$(CXX) -o $(LCP_FASTA_MEM).o
	@echo "Linking lcptoolsS library and binary files"
	$(GXX) $(CXXFLAGS) -o $(LCP_FASTA_MEM) $(LCP_FASTA_MEM).o -L$(LIB_DIR) -llcptools -Wl,-rpath,$(LIB_DIR)
	rm $(LCP_FASTA_MEM).o
	@echo "Moving $(LCP_FASTA_MEM) to $(EXECUTABLE_DIR)"
	mv $(LCP_FASTA_MEM) $(EXECUTABLE_DIR)
	@echo "Preprocessing $(FASTA). Output will be put into $(OUT_DIR)/$(LCP_FASTA_MEM)-output.txt"
	$(TIME) $(EXECUTABLE_DIR)/$(LCP_FASTA_MEM) $(FASTA) > $(OUT_DIR)/$(LCP_FASTA_MEM)-output.txt 2>&1

######################################################################
# MINIMIZER
######################################################################
minimizerFasta: mkdir_bin check_fasta
	@echo "Compiling $(MINIMIZER_FASTA)$(CXX)"
	$(GXX) $(CXXFLAGS) -c $(MINIMIZER_FASTA)$(CXX) -o $(MINIMIZER_FASTA).o
	$(GXX) $(CXXFLAGS) -o $(MINIMIZER_FASTA) $(MINIMIZER_FASTA).o
	rm $(MINIMIZER_FASTA).o
	@echo "Moving $(MINIMIZER_FASTA) to $(EXECUTABLE_DIR)"
	mv $(MINIMIZER_FASTA) $(EXECUTABLE_DIR)
	@echo "Preprocessing $(FASTA). Output will be put into $(OUT_DIR)/$(MINIMIZER_FASTA)-output.txt"
	$(TIME) $(EXECUTABLE_DIR)/$(MINIMIZER_FASTA) $(FASTA) > $(OUT_DIR)/$(MINIMIZER_FASTA)-output.txt 2>&1

######################################################################
# SYNCMER
######################################################################
syncmerFasta: mkdir_bin check_fasta
	@echo "Compiling $(SYNCMER_FASTA)$(CXX)"
	$(GXX) $(CXXFLAGS) -c $(SYNCMER_FASTA)$(CXX) -o $(SYNCMER_FASTA).o
	$(GXX) $(CXXFLAGS) -o $(SYNCMER_FASTA) $(SYNCMER_FASTA).o -L$(LIB_DIR) -llcptoolsS -Wl,-rpath,$(LIB_DIR)
	rm $(SYNCMER_FASTA).o
	@echo "Moving $(SYNCMER_FASTA) to $(EXECUTABLE_DIR)"
	mv $(SYNCMER_FASTA) $(EXECUTABLE_DIR)
	@echo "Preprocessing $(FASTA). Output will be put into $(OUT_DIR)/$(SYNCMER_FASTA)-output.txt"
	$(TIME) $(EXECUTABLE_DIR)/$(SYNCMER_FASTA) $(FASTA) > $(OUT_DIR)/$(SYNCMER_FASTA)-output.txt 2>&1