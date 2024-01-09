USERNAME := akmuhammet

# Directories
CURRENT_DIR := $(shell pwd)
TEMP_DIR := /tmp/$(USERNAME)
LOCAL_DIR := /home/$(USERNAME)
DATA_DIR := $(LOCAL_DIR)/data
EXECUTABLE_DIR := $(CURRENT_DIR)/../bin
RESULTS_DIR := $(CURRENT_DIR)/../results

# Data
HUMAN_V38_FASTA := human_v38.fasta
PBSIM_MAF := sd_0001.maf
PBSIM_FASTQ := sd_0001.fastq

# Data paths
HUMAN_V38_FASTA_PATH := $(TEMP_DIR)/$(HUMAN_V38_FASTA)
PBSIM_MAF_PATH := $(TEMP_DIR)/$(PBSIM_MAF)

# Extension
CXX := .cpp

# Compiler
GXX := g++

# Programs
CPP_LCP_FASTA := cpp-lcp-fasta
CPP_LCP_FASTQ_ERROR := cpp-lcp-fastq-error
CPP_MINIMIZER_FASTA := cpp-minimizer-fasta
CPP_MINIMIZER_FASTQ_ERROR := cpp-minimizer-fastq-error
PROCESS_MAF := process-maf

create_temp:
	mkdir -p $(TEMP_DIR)
	echo "Diretory created.";

create_executable:
	mkdir -p $(EXECUTABLE_DIR)

create_results:
	mkdir -p $(RESULTS_DIR)

######################################################################
# HELPERS
######################################################################
processMaf: create_temp
	$(GXX) -o $(PROCESS_MAF) $(PROCESS_MAF)$(CXX);
	mv $(PROCESS_MAF) $(EXECUTABLE_DIR);
	time $(EXECUTABLE_DIR)/$(PROCESS_MAF) $(CURRENT_DIR)/test.maf

moveFasta: create_temp
	if ! [ -f $(HUMAN_V38_FASTA_PATH) ]; then \
		cp -L $(DATA_DIR)/$(HUMAN_V38_FASTA) $(TEMP_DIR); \
	fi

moveMaf: create_temp
	if ! [ -f $(PBSIM_MAF_PATH) ]; then \
		cp -L $(DATA_DIR)/$(PBSIM_MAF) $(TEMP_DIR); \
	fi

######################################################################
# LCP
######################################################################
lcpFasta: moveFasta create_executable create_results
	$(GXX) -o $(CPP_LCP_FASTA) $(CPP_LCP_FASTA)$(CXX) && \
	mv $(CPP_LCP_FASTA) $(EXECUTABLE_DIR) && \
	time $(EXECUTABLE_DIR)/$(CPP_LCP_FASTA) $(HUMAN_V38_FASTA_PATH) > $(TEMP_DIR)/$(CPP_LCP_FASTA)-output.txt && \
	mv $(TEMP_DIR)/$(CPP_LCP_FASTA)-output.txt $(RESULTS_DIR)/;

lcpFastq: moveMaf create_executable create_results
	$(GXX) -o $(CPP_LCP_FASTQ_ERROR) $(CPP_LCP_FASTQ_ERROR)$(CXX) && \
	mv $(CPP_LCP_FASTQ_ERROR) $(EXECUTABLE_DIR) && \
	time $(EXECUTABLE_DIR)/$(CPP_LCP_FASTQ_ERROR) $(PBSIM_MAF) > $(TEMP_DIR)/$(CPP_LCP_FASTQ_ERROR)-output.txt && \
	mv $(TEMP_DIR)/$(CPP_LCP_FASTQ_ERROR)-output.txt $(RESULTS_DIR)/;

######################################################################
# MINIMIZER
######################################################################
minimizerFasta: moveFasta create_executable create_results
	$(GXX) -o $(CPP_MINIMIZER_FASTA) $(CPP_MINIMIZER_FASTA)$(CXX) && \
	mv $(CPP_MINIMIZER_FASTA) $(EXECUTABLE_DIR) && \
	time $(EXECUTABLE_DIR)/$(CPP_MINIMIZER_FASTA) $(HUMAN_V38_FASTA_PATH) > $(TEMP_DIR)/$(CPP_MINIMIZER_FASTA)-output.txt && \
	mv $(TEMP_DIR)/$(CPP_MINIMIZER_FASTA)-output.txt $(RESULTS_DIR)/;

minimizerFastq: moveMaf create_executable create_results
	$(GXX) -o $(CPP_MINIMIZER_FASTQ_ERROR) $(CPP_MINIMIZER_FASTQ_ERROR)$(CXX) && \
	mv cpp-minimizer-fastq-error ../bin && \
	time ../bin/cpp-minimizer-fastq-error ~/data/pbsim/sd_0001.maf > ../results/output.minimizer.fastq.pbsim.txt

######################################################################
# UHT
######################################################################
uhtFasta: moveFasta create_executable create_results
	$(GXX) -o cpp-uht-fasta cpp-uht-fasta.cpp && \
	mv cpp-uht-fasta ../bin && \
	time ../bin/cpp-uht-fasta ~/links/human_v38.fasta > ../results/output.uht.fasta.txt

uhtFastq: moveMaf create_executable create_results
	$(GXX) -o cpp-uht-fastq-error cpp-uht-fastq-error.cpp && \
	mv cpp-uht-fastq-error ../bin && \
	time ../bin/cpp-uht-fastq-error ~/data/pbsim/sd_0001.maf > ../results/output.uht.fastq.pbsim.txt