HUMAN_V38_FASTA := human_v38.fasta
PBSIM_MAF := sd_merged_85.maf.gz
PBSIM_FASTQ := sd_merged_85.fastq.gz
LCP_LEVEL := 4

# Programs
CPP_LCP_FASTA := cpp-lcp-fasta
CPP_LCP_FASTQ_ERROR := cpp-lcp-fastq-error
CPP_MINIMIZER_FASTA := cpp-minimizer-fasta
CPP_MINIMIZER_FASTQ_ERROR := cpp-minimizer-fastq-error
PROCESS_MAF := process-maf
FASTQ_TO_FASTQ := fastq-to-fasta
GOLD_STANDARD_GFA := gold-standard-gfa
GOLD_STANDARD_FASTQ := gold-standard-fastq

# Directories
CURRENT_DIR := $(shell pwd)
DATA_DIR := $(CURRENT_DIR)/../data
EXECUTABLE_DIR := $(CURRENT_DIR)/../bin
RESULTS_DIR := $(CURRENT_DIR)/../results

# Data paths
HUMAN_V38_FASTA_PATH := $(DATA_DIR)/$(HUMAN_V38_FASTA)
PBSIM_MAF_PATH := $(DATA_DIR)/$(PBSIM_MAF)
PBSIM_FASTQ_PATH := $(DATA_DIR)/$(PBSIM_FASTQ)

# Extension
CXX := .cpp

# Compiler
GXX := g++
TIME := /usr/bin/time -v

.PHONY: all

all: MKDIR_BIN MKDIR_RESULTS TEST_DATA

MKDIR_BIN:
	@echo "Checking if $(EXECUTABLE_DIR) exists..."
	@if [ ! -d "$(EXECUTABLE_DIR)" ]; then \
		echo "$(EXECUTABLE_DIR) does not exist. Creating..."; \
		mkdir $(EXECUTABLE_DIR); \
	else \
		echo "$(EXECUTABLE_DIR) already exists."; \
	fi

MKDIR_RESULTS:
	@echo "Checking if $(RESULTS_DIR) exists..."
	@if [ ! -d "$(RESULTS_DIR)" ]; then \
		echo "$(RESULTS_DIR) does not exist. Creating..."; \
		mkdir $(RESULTS_DIR); \
	else \
		echo "$(RESULTS_DIR) already exists."; \
	fi

TEST_DATA:
	@if [ ! -f $(HUMAN_V38_FASTA_PATH) ]; then \
		echo "File $(HUMAN_V38_FASTA_PATH) does not exist. Exiting."; \
		exit 1; \
	fi
	@echo "File $(HUMAN_V38_FASTA_PATH) exists."
	@if [ ! -f $(PBSIM_MAF_PATH) ]; then \
		echo "File $(PBSIM_MAF_PATH) does not exist. Exiting."; \
		exit 1; \
	fi
	@echo "File $(PBSIM_MAF_PATH) exists."
	@if [ ! -f $(PBSIM_FASTQ_PATH) ]; then \
		echo "File $(PBSIM_FASTQ_PATH) does not exist. Exiting."; \
		exit 1; \
	fi
	@echo "File $(PBSIM_FASTQ_PATH) exists."

######################################################################
# HELPERS
######################################################################
processMaf: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(PROCESS_MAF)$(CXX)"
	$(GXX) -o $(PROCESS_MAF) $(PROCESS_MAF)$(CXX) -lz
	@echo "Moving $(PROCESS_MAF) to $(EXECUTABLE_DIR)"
	mv $(PROCESS_MAF) $(EXECUTABLE_DIR)
	@for filepath in $(DATA_DIR)/sd_merged_*.maf.gz; do \
		file=$$(basename $$filepath); \
		pname=$$(echo $$file | sed 's/\.maf.gz/_processed.maf.gz/'); \
		echo "Processing $$file"; \
		if [ -f $(RESULTS_DIR)/$$pname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$pname"; \
			rm -f $(RESULTS_DIR)/$$pname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$pname"; \
		$(TIME) $(EXECUTABLE_DIR)/$(PROCESS_MAF) $$filepath $(RESULTS_DIR)/$$pname; \
	done

fastqToFastq: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(FASTQ_TO_FASTQ)$(CXX)"
	$(GXX) -o $(FASTQ_TO_FASTQ) $(FASTQ_TO_FASTQ)$(CXX) -lz
	@echo "Moving $(FASTQ_TO_FASTQ) to $(EXECUTABLE_DIR)"
	mv $(FASTQ_TO_FASTQ) $(EXECUTABLE_DIR)
	@for filepath in $(DATA_DIR)/sd_merged_*.fastq.gz; do \
		file=$$(basename $$filepath); \
		rname=$$(echo $$file | sed 's/\.fastq.gz/.fasta.gz/'); \
		echo "Processing $$file"; \
		if [ -f $(RESULTS_DIR)/$$rname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$rname"; \
			rm -f $(RESULTS_DIR)/$$rname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$rname"; \
		$(TIME) $(EXECUTABLE_DIR)/$(FASTQ_TO_FASTQ) $$filepath $(RESULTS_DIR)/$$rname; \
	done

goldStandartGfa: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(GOLD_STANDARD_GFA)$(CXX)"
	$(GXX) -o $(GOLD_STANDARD_GFA) $(GOLD_STANDARD_GFA)$(CXX) -lz
	@echo "Moving $(GOLD_STANDARD_GFA) to $(EXECUTABLE_DIR)"
	mv $(GOLD_STANDARD_GFA) $(EXECUTABLE_DIR)
	@for filepath in $(DATA_DIR)/sd_merged_*_processed.maf.gz; do \
		file=$$(basename $$filepath); \
		rname=$$(echo $$file | sed 's/\.maf.gz/.gfa/'); \
		echo "Processing $$file"; \
		if [ -f $(RESULTS_DIR)/$$rname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$rname"; \
			rm -f $(RESULTS_DIR)/$$rname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$rname"; \
		$(TIME) $(EXECUTABLE_DIR)/$(GOLD_STANDARD_GFA) $$filepath $(RESULTS_DIR)/$$rname; \
	done

goldStandartFastq: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(GOLD_STANDARD_FASTQ)$(CXX)"
	$(GXX) -o $(GOLD_STANDARD_FASTQ) $(GOLD_STANDARD_FASTQ)$(CXX) -lz
	@echo "Moving $(GOLD_STANDARD_FASTQ) to $(EXECUTABLE_DIR)"
	mv $(GOLD_STANDARD_FASTQ) $(EXECUTABLE_DIR)
	@for filepath in $(DATA_DIR)/sd_merged_*_processed.maf.gz; do \
		file=$$(basename $$filepath); \
		rname=(echo $$file | sed 's/\.maf.gz/_gold.fastq.gz/'); \
		echo "Processing $$file"; \
		if [ -f $(RESULTS_DIR)/$$rname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$rname"; \
			rm -f $(RESULTS_DIR)/$$rname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$rname"; \
		$(TIME) $(EXECUTABLE_DIR)/$(GOLD_STANDARD_FASTQ) $$filepath $(RESULTS_DIR)/$$rname; \
	done

######################################################################
# LCP
######################################################################
lcpFasta: MKDIR_BIN MKDIR_RESULTS
	$(GXX) -o $(CPP_LCP_FASTA) $(CPP_LCP_FASTA)$(CXX); \
	mv $(CPP_LCP_FASTA) $(EXECUTABLE_DIR); \
	$(TIME) $(EXECUTABLE_DIR)/$(CPP_LCP_FASTA) $(HUMAN_V38_FASTA_PATH) | gzip > $(RESULTS_DIR)/$(CPP_LCP_FASTA)-output.txt

lcpFastq: MKDIR_BIN MKDIR_RESULTS
	$(GXX) -o $(CPP_LCP_FASTQ_ERROR) $(CPP_LCP_FASTQ_ERROR)$(CXX); \
	mv $(CPP_LCP_FASTQ_ERROR) $(EXECUTABLE_DIR); \
	$(TIME) $(EXECUTABLE_DIR)/$(CPP_LCP_FASTQ_ERROR) $(PBSIM_MAF_PROCESSED) $(LCP_LEVEL)

######################################################################
# MINIMIZER
######################################################################
minimizerFasta: MKDIR_BIN MKDIR_RESULTS
	$(GXX) -o $(CPP_MINIMIZER_FASTA) $(CPP_MINIMIZER_FASTA)$(CXX) && \
	mv $(CPP_MINIMIZER_FASTA) $(EXECUTABLE_DIR) && \
	time $(EXECUTABLE_DIR)/$(CPP_MINIMIZER_FASTA) $(HUMAN_V38_FASTA_PATH) > $(TEMP_DIR)/$(CPP_MINIMIZER_FASTA)-output.txt && \
	mv $(TEMP_DIR)/$(CPP_MINIMIZER_FASTA)-output.txt $(RESULTS_DIR)/;

minimizerFastq: MKDIR_BIN MKDIR_RESULTS
	$(GXX) -o $(CPP_MINIMIZER_FASTQ_ERROR) $(CPP_MINIMIZER_FASTQ_ERROR)$(CXX) && \
	mv cpp-minimizer-fastq-error ../bin && \
	time ../bin/cpp-minimizer-fastq-error ~/data/pbsim/sd_0001.maf > ../results/output.minimizer.fastq.pbsim.txt