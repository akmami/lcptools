FASTA := hg38.fa
FASTQ := *.fastq.gz
LCP_LEVEL := 4

# Programs
CPP_LCP_FASTA := cpp-lcp-fasta
CPP_LCP_FASTQ_ERROR := cpp-lcp-fastq-error
CPP_MINIMIZER_FASTA := cpp-minimizer-fasta
CPP_MINIMIZER_FASTQ_ERROR := cpp-minimizer-fastq-error

# Directories
CURRENT_DIR := $(shell pwd)
DATA_DIR := $(CURRENT_DIR)/../data
EXECUTABLE_DIR := $(CURRENT_DIR)/../bin
RESULTS_DIR := $(CURRENT_DIR)/../results
OUT_DIR := $(CURRENT_DIR)/../out

# Extension
CXX := .cpp

# Compiler
GXX := g++
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

check_fasta:
	@if [ ! -f $(DATA_DIR)/$(FASTA) ]; then \
		echo "File $(DATA_DIR)/$(FASTA) does not exist. Exiting."; \
		exit 1; \
	fi

check_fastq:
	@if ! ls $(DATA_DIR)/$(FASTQ) 1> /dev/null; then \
		echo "No .fastq.gz files found. Exiting."; \
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
lcpFasta: mkdir_bin mkdir_results check_fasta
	@echo "Compiling $(CPP_LCP_FASTA)$(CXX)";
	$(GXX) -Wall -o $(CPP_LCP_FASTA) $(CPP_LCP_FASTA)$(CXX);
	@echo "Moving $(CPP_LCP_FASTA) to $(EXECUTABLE_DIR)";
	mv $(CPP_LCP_FASTA) $(EXECUTABLE_DIR);
	@echo "Preprocessing $(DATA_DIR)/$(FASTA). Output will be put into $(OUT_DIR)/$(CPP_LCP_FASTA)-output.txt";
	$(TIME) $(EXECUTABLE_DIR)/$(CPP_LCP_FASTA) $(DATA_DIR)/$(FASTA) > $(OUT_DIR)/$(CPP_LCP_FASTA)-output.txt 2>&1;

lcpFastq: mkdir_bin mkdir_results check_fastq
	@echo "Compiling $(CPP_LCP_FASTQ_ERROR)$(CXX)";
	$(GXX) -Wall -o $(CPP_LCP_FASTQ_ERROR) $(CPP_LCP_FASTQ_ERROR)$(CXX) -lz
	@echo "Moving $(CPP_LCP_FASTQ_ERROR) to $(EXECUTABLE_DIR)";
	mv $(CPP_LCP_FASTQ_ERROR) $(EXECUTABLE_DIR);
	parallel "$(TIME) $(EXECUTABLE_DIR)/$(CPP_LCP_FASTQ_ERROR) $(DATA_DIR)/sd_merged_"{1}"_chr1.maf.gz $(RESULTS_DIR)/$(CPP_LCP_FASTQ_ERROR)-chr1-"{1}"-"{2}".txt "{2} " > $(OUT_DIR)/$(CPP_LCP_FASTQ_ERROR)-chr1-"{1}"-"{2}"-output.txt 2>&1;" ::: 85 90 95 99 ::: 2 3 4 5 6 7;

######################################################################
# MINIMIZER
######################################################################
minimizerFasta: mkdir_bin mkdir_results check_fasta
	@echo "Compiling $(CPP_MINIMIZER_FASTA)$(CXX)";
	$(GXX) -Wall -o $(CPP_MINIMIZER_FASTA) $(CPP_MINIMIZER_FASTA)$(CXX);
	@echo "Moving $(CPP_MINIMIZER_FASTA) to $(EXECUTABLE_DIR)";
	mv $(CPP_MINIMIZER_FASTA) $(EXECUTABLE_DIR);
	parallel --link "$(TIME) $(EXECUTABLE_DIR)/$(CPP_MINIMIZER_FASTA)" "$(DATA_DIR)/$(FASTA)" {1} {2} ">" "$(OUT_DIR)/$(CPP_MINIMIZER_FASTA)-k{1}-w{2}.txt 2>&1;"  ::: 4 10 16 ::: 8 20 32

minimizerFastq: mkdir_bin mkdir_results check_fastq
	@echo "Compiling $(CPP_MINIMIZER_FASTQ_ERROR)$(CXX)";
	$(GXX) -Wall -o $(CPP_MINIMIZER_FASTQ_ERROR) $(CPP_MINIMIZER_FASTQ_ERROR)$(CXX);
	@echo "Moving $(CPP_MINIMIZER_FASTQ_ERROR) to $(EXECUTABLE_DIR)";
	mv $(CPP_MINIMIZER_FASTQ_ERROR) $(EXECUTABLE_DIR);
	parallel "$(TIME) $(EXECUTABLE_DIR)/$(CPP_MINIMIZER_FASTQ_ERROR) $(DATA_DIR)/sd_merged_"{1}".maf.gz > $(OUT_DIR)/$(CPP_MINIMIZER_FASTQ_ERROR)-"{1}".txt 2>&1" ::: 85 90 95 99