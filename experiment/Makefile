FASTA ?=
LCP_LEVEL ?= 4

# Programs
LCP_FASTA := lcp-fasta
LCP_FASTA_MEM := lcp-fasta-mem

# Directories
CURRENT_DIR := $(shell pwd)
DATA_DIR := $(CURRENT_DIR)/../data
EXECUTABLE_DIR := $(CURRENT_DIR)/../bin
RESULTS_DIR := $(CURRENT_DIR)/../results
OUT_DIR := $(CURRENT_DIR)/../out
INCLUDE_DIR ?= $(CURRENT_DIR)/../include
LIB_DIR ?= $(CURRENT_DIR)/../lib

# Extension
CXX := .cpp

# Compiler
GXX := g++
CXXFLAGS = -std=c++11 -O2 -Wall -Wextra
TIME := /usr/bin/time -v

.PHONY: all

all: mkdir_bin mkdir_results mkdir_out

mkdir_bin:
	@echo "Checking if $(EXECUTABLE_DIR) exists..."
	@if [ ! -d "$(EXECUTABLE_DIR)" ]; then \
		echo "$(EXECUTABLE_DIR) does not exist. Creating..."; \
		mkdir $(EXECUTABLE_DIR); \
	else \
		echo "$(EXECUTABLE_DIR) already exists."; \
	fi

mkdir_results:
	@echo "Checking if $(RESULTS_DIR) exists..."
	@if [ ! -d "$(RESULTS_DIR)" ]; then \
		echo "$(RESULTS_DIR) does not exist. Creating..."; \
		mkdir $(RESULTS_DIR); \
	else \
		echo "$(RESULTS_DIR) already exists."; \
	fi

mkdir_out:
	@echo "Checking if $(OUT_DIR) exists..."
	@if [ ! -d "$(OUT_DIR)" ]; then \
		echo "$(OUT_DIR) does not exist. Creating..."; \
		mkdir $(OUT_DIR); \
	else \
		echo "$(OUT_DIR) already exists."; \
	fi

check_lcptools:
	@if [ ! -d "$(INCLUDE_DIR)" ]; then \
		echo "$(INCLUDE_DIR) does not exist. Please compile lcptools to headers to $(INCLUDE_DIR)"; \
		exit 1; \
	fi
	@if [ ! -d "$(LIB_DIR)" ]; then \
		echo "$(LIB_DIR) does not exist. Please compile lcptools and put library to $(LIB_DIR)"; \
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
	@if [ -d "$(RESULTS_DIR)" ]; then \
		echo "Removing $(RESULTS_DIR)"; \
		rm -r $(RESULTS_DIR); \
	fi
	@echo "Cleaned."

######################################################################
# LCP
######################################################################
lcpFasta: mkdir_bin mkdir_results check_fasta check_lcptools
	@echo "Compiling $(LCP_FASTA)$(CXX)"
	$(GXX) $(CXXFLAGS) -DSTATS -I$(INCLUDE_DIR) -c $(LCP_FASTA)$(CXX) -o $(LCP_FASTA).o
	@echo "Linking lcptools library and binary files"
	$(GXX) $(CXXFLAGS) -o $(LCP_FASTA) $(LCP_FASTA).o -L$(LIB_DIR) -llcptools -Wl,-rpath,$(LIB_DIR)
	rm $(LCP_FASTA).o
	@echo "Moving $(LCP_FASTA) to $(EXECUTABLE_DIR)"
	mv $(LCP_FASTA) $(EXECUTABLE_DIR)
	@echo "Preprocessing $(FASTA). Output will be put into $(OUT_DIR)/$(LCP_FASTA)-output.txt"
	$(TIME) $(EXECUTABLE_DIR)/$(LCP_FASTA) $(FASTA) > $(OUT_DIR)/$(LCP_FASTA)-output.txt

lcpFastaMem: mkdir_bin mkdir_results check_fasta check_lcptools
	@echo "Compiling $(LCP_FASTA_MEM)$(CXX)"
	$(GXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -c $(LCP_FASTA_MEM)$(CXX) -o $(LCP_FASTA_MEM).o
	@echo "Linking lcptools library and binary files"
	$(GXX) $(CXXFLAGS) -o $(LCP_FASTA_MEM) $(LCP_FASTA_MEM).o -L$(LIB_DIR) -llcptools -Wl,-rpath,$(LIB_DIR)
	rm $(LCP_FASTA_MEM).o
	@echo "Moving $(LCP_FASTA_MEM) to $(EXECUTABLE_DIR)"
	mv $(LCP_FASTA_MEM) $(EXECUTABLE_DIR)
	@echo "Preprocessing $(FASTA). Output will be put into $(OUT_DIR)/$(LCP_FASTA_MEM)-output.txt"
	$(TIME) $(EXECUTABLE_DIR)/$(LCP_FASTA_MEM) $(FASTA) > $(OUT_DIR)/$(LCP_FASTA_MEM)-output.txt
