HUMAN_V38_FASTA := human_v38.fasta
PBSIM_MAF := *.maf.gz
PBSIM_FASTQ := *.fastq.gz
LCP_LEVEL := 4

# Programs
CPP_LCP_FASTA := cpp-lcp-fasta
CPP_LCP_FASTQ_ERROR := cpp-lcp-fastq-error
CPP_MINIMIZER_FASTA := cpp-minimizer-fasta
CPP_MINIMIZER_FASTQ_ERROR := cpp-minimizer-fastq-error
FASTQ_TO_FASTA := fastq-to-fasta
GOLD_STANDARD_GFA := gold-standard-gfa
LCP_TRIE_BASED := lcp-trie-based-stat
LCP_INVERTED_BASED := lcp-inverted-based
COMPARE_GFA := compare-gfa
EXTRACT_CHROM := extract-chrom

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
# HELPERS
######################################################################
fastqToFasta: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(FASTQ_TO_FASTA)$(CXX)";
	$(GXX) -Wall -o $(FASTQ_TO_FASTA) $(FASTQ_TO_FASTA)$(CXX) -lz;
	@echo "Moving $(FASTQ_TO_FASTA) to $(EXECUTABLE_DIR)";
	mv $(FASTQ_TO_FASTA) $(EXECUTABLE_DIR);
	@find $(DATA_DIR) -name '$(PBSIM_FASTQ)' | parallel ' \
		file=$$(basename {}); \
		rname=$$(echo $$file | sed 's/\.fastq.gz/.fasta.gz/'); \
		out_file=$(FASTQ_TO_FASTA)-$$(echo $$file | sed 's/\.fastq.gz/-out.txt/'); \
		echo "Processing $$file" > $(OUT_DIR)/$${out_file}; \
		if [ -f $(RESULTS_DIR)/$$rname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
			rm -f $(RESULTS_DIR)/$$rname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
		$(TIME) $(EXECUTABLE_DIR)/$(FASTQ_TO_FASTA) {} $(RESULTS_DIR)/$$rname >> $(OUT_DIR)/$${out_file} 2>&1 \
	'

goldStandartGfa: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(GOLD_STANDARD_GFA)$(CXX)";
	$(GXX) -Wall -o $(GOLD_STANDARD_GFA) $(GOLD_STANDARD_GFA)$(CXX) -lz;
	@echo "Moving $(GOLD_STANDARD_GFA) to $(EXECUTABLE_DIR)";
	mv $(GOLD_STANDARD_GFA) $(EXECUTABLE_DIR);
	@find $(DATA_DIR) -name '$(PBSIM_MAF)' | parallel ' \
		file=$$(basename {}); \
		rname=$$(echo $$file | sed 's/\.maf.gz/_gold.gfa/'); \
		out_file=$(GOLD_STANDARD_GFA)-$$(echo $$file | sed 's/\.maf.gz/-out.txt/'); \
		echo "Processing $$file" > $(OUT_DIR)/$${out_file};  \
		if [ -f $(RESULTS_DIR)/$$rname ]; then \
			echo "Removing existing processed file $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
			rm -f $(RESULTS_DIR)/$$rname; \
        fi; \
		echo "Preprocessing $$file. Output will be put into $(RESULTS_DIR)/$$rname" >> $(OUT_DIR)/$${out_file}; \
		$(TIME) $(EXECUTABLE_DIR)/$(GOLD_STANDARD_GFA) {} $(RESULTS_DIR)/$$rname --simple >> $(OUT_DIR)/$${out_file} 2>&1 \
	'

######################################################################
# LCP
######################################################################
lcpFasta: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(CPP_LCP_FASTA)$(CXX)";
	$(GXX) -Wall -o $(CPP_LCP_FASTA) $(CPP_LCP_FASTA)$(CXX);
	@echo "Moving $(CPP_LCP_FASTA) to $(EXECUTABLE_DIR)";
	mv $(CPP_LCP_FASTA) $(EXECUTABLE_DIR);
	@echo "Preprocessing $(HUMAN_V38_FASTA_PATH). Output will be put into $(RESULTS_DIR)/$(CPP_LCP_FASTA)-output.txt"; \
	$(TIME) $(EXECUTABLE_DIR)/$(CPP_LCP_FASTA) $(HUMAN_V38_FASTA_PATH) > $(RESULTS_DIR)/$(CPP_LCP_FASTA)-output.txt 2>&1;

lcpFastq: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(CPP_LCP_FASTQ_ERROR)$(CXX)";
	$(GXX) -Wall -o $(CPP_LCP_FASTQ_ERROR) $(CPP_LCP_FASTQ_ERROR)$(CXX) -lz
	@echo "Moving $(CPP_LCP_FASTQ_ERROR) to $(EXECUTABLE_DIR)";
	mv $(CPP_LCP_FASTQ_ERROR) $(EXECUTABLE_DIR);
	parallel "$(TIME) $(EXECUTABLE_DIR)/$(CPP_LCP_FASTQ_ERROR) $(DATA_DIR)/extract-chrom_85_chrom_1.fastq.gz"{1}".maf.gz "{2}" > $(RESULTS_DIR)/$(CPP_LCP_FASTQ_ERROR)-"{1}"-"{2}".txt 2>&1" ::: 85 90 95 99 ::: 2 3 4 5 6 7

######################################################################
# MINIMIZER
######################################################################
minimizerFasta: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(CPP_MINIMIZER_FASTA)$(CXX)";
	$(GXX) -Wall -g -o $(CPP_MINIMIZER_FASTA) $(CPP_MINIMIZER_FASTA)$(CXX);
	@echo "Moving $(CPP_MINIMIZER_FASTA) to $(EXECUTABLE_DIR)";
	mv $(CPP_MINIMIZER_FASTA) $(EXECUTABLE_DIR);
	parallel --link "$(TIME) $(EXECUTABLE_DIR)/$(CPP_MINIMIZER_FASTA)" "$(HUMAN_V38_FASTA_PATH)" {1} {2} ">" "$(RESULTS_DIR)/$(CPP_MINIMIZER_FASTA)-k{1}-w{2}.txt 2>&1;"  ::: 4 10 16 ::: 8 20 32

minimizerFastq: MKDIR_BIN MKDIR_RESULTS
	@echo "Compiling $(CPP_MINIMIZER_FASTQ_ERROR)$(CXX)";
	$(GXX) -Wall -o $(CPP_MINIMIZER_FASTQ_ERROR) $(CPP_MINIMIZER_FASTQ_ERROR)$(CXX);
	@echo "Moving $(CPP_MINIMIZER_FASTQ_ERROR) to $(EXECUTABLE_DIR)";
	mv $(CPP_MINIMIZER_FASTQ_ERROR) $(EXECUTABLE_DIR);
	parallel "$(TIME) $(EXECUTABLE_DIR)/$(CPP_MINIMIZER_FASTQ_ERROR) $(DATA_DIR)/sd_merged_"{1}".maf.gz > $(RESULTS_DIR)/$(CPP_MINIMIZER_FASTQ_ERROR)-"{1}".txt 2>&1" ::: 85 90 95 99

######################################################################
# INDEXING
######################################################################
lcpTrieIndexing:
	@echo "Compiling $(LCP_TRIE_BASED)$(CXX)";
	$(GXX) -Wall -o $(LCP_TRIE_BASED) $(LCP_TRIE_BASED)$(CXX) -lz; 
	@echo "Moving $(LCP_TRIE_BASED) to $(EXECUTABLE_DIR)";
	mv $(LCP_TRIE_BASED) $(EXECUTABLE_DIR) && \
	parallel "$(TIME) $(EXECUTABLE_DIR)/$(LCP_TRIE_BASED) $(DATA_DIR)/sd_merged_"{}".fastq.gz $(RESULTS_DIR)/sd_merged_"{}"_trie.gfa > $(RESULTS_DIR)/$(LCP_TRIE_BASED)-"{}".txt 2>&1" ::: 85 90 95 99;

lcpInvertedIndexing:;
	@echo "Compiling $(LCP_INVERTED_BASED)$(CXX)";
	$(GXX) -Wall -o $(LCP_INVERTED_BASED) $(LCP_INVERTED_BASED)$(CXX) -lz;
	@echo "Moving $(LCP_INVERTED_BASED) to $(EXECUTABLE_DIR)";
	mv $(LCP_INVERTED_BASED) $(EXECUTABLE_DIR);
	$(TIME) $(EXECUTABLE_DIR)/$(LCP_INVERTED_BASED) $(DATA_DIR)/simulated.fastq.gz $(RESULTS_DIR)/simulated-inverted-result.gfa;

######################################################################
# OTHER
######################################################################
compareGFAs:
	@echo "Compiling $(COMPARE_GFA)$(CXX)";
	$(GXX) -Wall -g -o $(COMPARE_GFA) $(COMPARE_GFA)$(CXX);
	@echo "Moving $(COMPARE_GFA) to $(EXECUTABLE_DIR)";
	mv $(COMPARE_GFA) $(EXECUTABLE_DIR) && \
	$(TIME) $(EXECUTABLE_DIR)/$(COMPARE_GFA) $(RESULTS_DIR)/sd_merged_99_gold.gfa $(RESULTS_DIR)/sd_merged_99_trie.gfa;

extractChrom:
	@echo "Compiling $(EXTRACT_CHROM)$(CXX)";
	$(GXX) -Wall -o $(EXTRACT_CHROM) $(EXTRACT_CHROM)$(CXX) -lz;
	@echo "Moving $(EXTRACT_CHROM) to $(EXECUTABLE_DIR)";
	mv $(EXTRACT_CHROM) $(EXECUTABLE_DIR) && \
	parallel --link "$(TIME) $(EXECUTABLE_DIR)/$(EXTRACT_CHROM) $(DATA_DIR)/sd_merged_"{1}".maf.gz $(RESULTS_DIR)/$(EXTRACT_CHROM)_"{1}"_chrom_"{2}".maf.gz "{2} ::: 85 90 95 99 ::: 1;
