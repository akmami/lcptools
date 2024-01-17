HUMAN_V38_FASTA := human_v38.fasta
PBSIM_MAF := *.maf.gz
PBSIM_FASTQ := *.fastq.gz
PREFIX := sd_merged_
LCP_LEVEL := 4

# Programs
CPP_LCP_FASTA := cpp-lcp-fasta
CPP_LCP_FASTQ_ERROR := cpp-lcp-fastq-error
CPP_MINIMIZER_FASTA := cpp-minimizer-fasta
CPP_MINIMIZER_FASTQ_ERROR := cpp-minimizer-fastq-error
PROCESS_MAF := process-maf
FASTQ_TO_FASTQ := fastq-to-fasta
GOLD_STANDARD_GFA := gold-standard-gfa

# Directories
CURRENT_DIR := $(shell pwd)
DATA_DIR := $(CURRENT_DIR)/../data
EXECUTABLE_DIR := $(CURRENT_DIR)/../bin
RESULTS_DIR := $(CURRENT_DIR)/../results
OUT_DIR := $(CURRENT_DIR)/../out

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

all: MKDIR_BIN MKDIR_RESULTS MKDIR_OUT TEST_DATA

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

MKDIR_OUT:
	@echo "Checking if $(OUT_DIR) exists..."
	@if [ ! -d "$(OUT_DIR)" ]; then \
		echo "$(OUT_DIR) does not exist. Creating..."; \
		mkdir $(OUT_DIR); \
	else \
		echo "$(OUT_DIR) already exists."; \
	fi

TEST_DATA:
	@if [ ! -f $(HUMAN_V38_FASTA_PATH) ]; then \
		echo "File $(HUMAN_V38_FASTA_PATH) does not exist. Exiting."; \
		exit 1; \
	fi
	@echo "File $(HUMAN_V38_FASTA_PATH) exists."
	@if ! ls $(PBSIM_MAF_PATH) 1> /dev/null; then \
		echo "No .maf.gz files found. Exiting."; \
		exit 1; \
	fi
	@echo "At least one maf.gz file exists."
	@if ! ls $(PBSIM_FASTQ_PATH) 1> /dev/null; then \
		echo "No .fastq.gz files found. Exiting."; \
		exit 1; \
	fi
	@echo "At least one fastq.gz file exists."

######################################################################
# HELPERS
######################################################################
processMaf: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(PROCESS_MAF)$(CXX)"
	$(GXX) -o $(PROCESS_MAF) $(PROCESS_MAF)$(CXX) -lz
	@echo "Moving $(PROCESS_MAF) to $(EXECUTABLE_DIR)"
	mv $(PROCESS_MAF) $(EXECUTABLE_DIR)
	@echo "Processing files in parallel"
	@find $(DATA_DIR) -name '$(PREFIX)*.maf.gz' | parallel ' \
		file=$$(basename {}); \
		pname=$$(echo $$file | sed 's/\.maf.gz/_processed.maf.gz/'); \
		out_file=$(PROCESS_MAF)-$${file#$(PREFIX)}; \
		out_file=$$(echo $$out_file | sed 's/\.maf.gz/-out.txt/'); \
		echo "Processing $${file}" > $(OUT_DIR)/$${out_file}; \
		if [ -f $(RESULTS_DIR)/$${pname} ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$${pname}" >> $(OUT_DIR)/$${out_file}; \
			rm -f $(RESULTS_DIR)/$${pname}; \
		fi; \
		echo "Preprocessing $${file}. Output will be put into $(RESULTS_DIR)/$${pname}" >> $(OUT_DIR)/$${out_file}; \
		$(TIME) $(EXECUTABLE_DIR)/$(PROCESS_MAF) {} $(RESULTS_DIR)/$${pname} >> $(OUT_DIR)/$${out_file} 2>&1 \
	'

fastqToFastq: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(FASTQ_TO_FASTQ)$(CXX)"
	$(GXX) -o $(FASTQ_TO_FASTQ) $(FASTQ_TO_FASTQ)$(CXX) -lz
	@echo "Moving $(FASTQ_TO_FASTQ) to $(EXECUTABLE_DIR)"
	mv $(FASTQ_TO_FASTQ) $(EXECUTABLE_DIR)
	@find $(DATA_DIR) -name '$(PREFIX)*.fastq.gz' | parallel ' \
		file=$$(basename $$filepath); \
		rname=$$(echo $$file | sed 's/\.fastq.gz/.fasta.gz/'); \
		out_file=$(FASTQ_TO_FASTQ)-$${file#$(PREFIX)}; \
		out_file=$$(echo $$out_file | sed 's/\.fastq.gz/-out.txt/'); \
		echo "Processing $$file" > $(OUT_DIR)/$${out_file}; \
		if [ -f $(RESULTS_DIR)/$$rname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
			rm -f $(RESULTS_DIR)/$$rname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
		$(TIME) $(EXECUTABLE_DIR)/$(FASTQ_TO_FASTQ) $$filepath $(RESULTS_DIR)/$$rname >> $(OUT_DIR)/$${out_file} 2>&1 \
	'

goldStandartGfa: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(GOLD_STANDARD_GFA)$(CXX)"
	$(GXX) -o $(GOLD_STANDARD_GFA) $(GOLD_STANDARD_GFA)$(CXX) -lz
	@echo "Moving $(GOLD_STANDARD_GFA) to $(EXECUTABLE_DIR)"
	mv $(GOLD_STANDARD_GFA) $(EXECUTABLE_DIR)
	@find $(DATA_DIR) -name '$(PREFIX)*_processed.maf.gz' | parallel ' \
		file=$$(basename $$filepath); \
		rname=$$(echo $$file | sed 's/\.maf.gz/.gfa/'); \
		out_file=$(GOLD_STANDARD_GFA)-$${file#$(PREFIX)}; \
		out_file=$$(echo $$out_file | sed 's/\_processed.maf.gz/-out.txt/'); \
		echo "Processing $$file" > $(OUT_DIR)/$${out_file};  \
		if [ -f $(RESULTS_DIR)/$$rname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
			rm -f $(RESULTS_DIR)/$$rname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
		$(TIME) $(EXECUTABLE_DIR)/$(GOLD_STANDARD_GFA) $$filepath $(RESULTS_DIR)/$$rname >> $(OUT_DIR)/$${out_file}; \
	'

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