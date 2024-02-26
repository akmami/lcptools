LCP_LEVEL := 4

# Programs
COMPARE_GENOMES := compare-genomes

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
	@if [ ! -f $(DATA_DIR)/$(HUMAN_V38_FASTA) ]; then \
		echo "File $(DATA_DIR)/$(HUMAN_V38_FASTA) does not exist. Exiting."; \
		exit 1; \
	fi
	@echo "File $(DATA_DIR)/$(HUMAN_V38_FASTA) exists."
	@if ! ls $(DATA_DIR)/$(PBSIM_MAF) 1> /dev/null; then \
		echo "No .maf.gz files found. Exiting."; \
		exit 1; \
	fi
	@echo "At least one maf.gz file exists."
	@if ! ls $(DATA_DIR)/$(PBSIM_FASTQ) 1> /dev/null; then \
		echo "No .fastq.gz files found. Exiting."; \
		exit 1; \
	fi
	@echo "At least one fastq.gz file exists."


######################################################################
# PROGRAM
######################################################################
compareGenomes:
	@echo "Compiling $(COMPARE_GENOMES)$(CXX)";
	$(GXX) -Wall -o $(COMPARE_GENOMES) $(COMPARE_GENOMES)$(CXX) -lz;
	@echo "Moving $(COMPARE_GENOMES) to $(EXECUTABLE_DIR)";
	mv $(COMPARE_GENOMES) $(EXECUTABLE_DIR);
	parallel "$(TIME) $(EXECUTABLE_DIR)/$(COMPARE_GENOMES) $(DATA_DIR)/sd_merged_"{1}"_gold_chr20.fastq.gz $(DATA_DIR)/sd_merged_"{1}"_simu_chr20.fastq.gz -l "{2} " > $(OUT_DIR)/$(COMPARE_GENOMES)-"{1}"-"{2}"-output.txt 2>&1 " ::: 85 ::: 6 7;

compareTrio:
	@echo "Compiling $(COMPARE_GENOMES)$(CXX)";
	$(GXX) -Wall -o $(COMPARE_GENOMES) $(COMPARE_GENOMES)$(CXX) -lz;
	@echo "Moving $(COMPARE_GENOMES) to $(EXECUTABLE_DIR)";
	mv $(COMPARE_GENOMES) $(EXECUTABLE_DIR);
	parallel ::: \
	"$(TIME) $(EXECUTABLE_DIR)/$(COMPARE_GENOMES) $(DATA_DIR)/HG002_son.fastq.gz $(DATA_DIR)/HG003_father.fastq.gz -l 7 --genome-1-len 3088269832 --genome-2-len 3088269832 > $(OUT_DIR)/$(COMPARE_GENOMES)-HG002_son-HG003_father-7-output.txt 2>&1" \
	"$(TIME) $(EXECUTABLE_DIR)/$(COMPARE_GENOMES) $(DATA_DIR)/HG002_son.fastq.gz $(DATA_DIR)/HG004_mother.fastq.gz -l 7 --genome-1-len 3088269832 --genome-2-len 3088269832 > $(OUT_DIR)/$(COMPARE_GENOMES)-HG002_son-HG003_mother-7-output.txt 2>&1" \
	"$(TIME) $(EXECUTABLE_DIR)/$(COMPARE_GENOMES) $(DATA_DIR)/HG003_father.fastq.gz $(DATA_DIR)/HG004_mother.fastq.gz -l 7 --genome-1-len 3088269832 --genome-2-len 3088269832 > $(OUT_DIR)/$(COMPARE_GENOMES)-HG003_father-HG004_mother-7-output.txt 2>&1"

