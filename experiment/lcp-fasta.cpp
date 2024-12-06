/**
 * @file    lcp-fasta.cpp
 * @brief   Analysis and Processing of Genomic Data
 *
 * This program is designed for in-depth analysis of genomic sequences. It reads
 * genomic data, processes it through multiple levels of analysis (defined by
 * LCP_LEVEL), and computes various statistics such as contiguous counts, distances,
 * and lengths of genomic sequences.
 *
 * The analysis is detailed and involves different stages, including the computation
 * of contiguous regions, distances between certain genomic features, and lengths of
 * various genomic segments. The results are then summarized to provide insights into
 * the genomic data structure.
 *
 * The program leverages a series of custom functions and data structures, operating
 * on large volumes of data with efficiency and accuracy in focus.
 */

#include "helper.cpp"
#include "lps.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#define USE_MAP true

/**
 * @brief Performs analysis of genomic data at a specified level.
 *
 * This function analyzes genomic data, specifically focusing on contiguous
 * counts, distances, and lengths at a given level of analysis. It iterates
 * through LCP cores, calculates relevant statistics, and stores them in
 * provided arrays and vectors. It supports analysis at different levels,
 * allowing for multi-layered (LCP level) examination of genomic sequences.
 *
 * @param level The level of analysis (used in multi-level processing).
 * @param contiguous_counts Array to store counts of contiguous genomic segments.
 * @param distances Multidimensional array to store position-based distances.
 * @param distancesXL Vector to store larger position-based distances.
 * @param lengths Multidimensional array to store lengths of genomic segments.
 * @param lengthsXL Vector to store larger lengths.
 * @param str Pointer to the genomic string being analyzed.
 */
void analyze(int level, int (&contiguous_counts)[LCP_LEVEL], int (&distances)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &distancesXL, int (&lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &lengthsXL, lcp::lps *str) {

	if (str->cores != nullptr) {

		bool isOverlapped = false;

		if (str->cores->begin()->end < DISTANCE_LENGTH + str->cores->begin()->start) {
			lengths[level][str->cores->begin()->end - str->cores->begin()->start] += 1;
		} else {
			lengthsXL[level].push_back(str->cores->begin()->end - str->cores->begin()->start);
		}

		for (std::vector<lcp::core>::iterator it = str->cores->begin() + 1; it < str->cores->end(); it++) {

			if ((it)->start <= (it - 1)->end) {
				contiguous_counts[level] += 1;

				if (!isOverlapped) {
					isOverlapped = true;
				}
			}

			if ((it)->start < DISTANCE_LENGTH + (it - 1)->start) {
				distances[level][(it)->start - (it - 1)->start]++;
			} else {
				distancesXL[level].push_back((it)->start - (it - 1)->start);
			}

			if ((it)->end < DISTANCE_LENGTH + (it)->start) {
				lengths[level][(it)->end - (it)->start] += 1;
			} else {
				lengthsXL[level].push_back((it)->end - (it)->start);
			}
		}

		if (isOverlapped) {
			contiguous_counts[level] += 1;
		}
	}
};

/**
 * @brief Processes genomic sequence data, tracks execution time, and analyzes
 * various statistics.
 *
 * This function processes a given genomic sequence by performing multiple levels
 * of LCP (Locally Consistent Parsing) analysis. It tracks the execution time for
 * each level of processing, updates various statistical arrays, and manages LCP core
 * extraction and deepening across specified levels. The function also calculates
 * contiguous counts, distances, and lengths for genomic segments, storing the
 * results in the provided arrays and vectors.
 *
 * @param sequence The genomic sequence (string) to be analyzed.
 * @param durations A vector storing the durations (in milliseconds) of each level's
 *                  processing time.
 * @param contiguous_counts An array storing the count of contiguous genomic segments
 *                           for each level.
 * @param core_counts An array storing the number of LCP cores found at each level.
 * @param distinct_core_counts An array string the number of distinct cores found at each level.
 * @param distances A multidimensional array storing position-based distances between
 *                  genomic segments at each level.
 * @param distancesXL A vector storing larger distances between genomic segments at each level.
 * @param lengths A multidimensional array storing the lengths of genomic segments at each level.
 * @param lengthsXL A vector storing larger lengths of genomic segments at each level.
 */
void process(std::string &sequence, std::vector<std::chrono::milliseconds> &durations, int (&contiguous_counts)[LCP_LEVEL], int (&core_counts)[LCP_LEVEL], int (&distinct_core_counts)[LCP_LEVEL], int (&distances)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &distancesXL, int (&lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &lengthsXL) {

	size_t initial_size = lcp::hash::str_map.size();

	auto start = std::chrono::high_resolution_clock::now();
	lcp::lps *str = new lcp::lps(sequence, USE_MAP);

	auto extraction_end = std::chrono::high_resolution_clock::now();
	distinct_core_counts[0] += lcp::hash::str_map.size() - initial_size;
	durations[0] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(extraction_end - start).count());
	if (str->cores != nullptr) {
		core_counts[0] += str->cores->size();
	}

	analyze(0, contiguous_counts, distances, distancesXL, lengths, lengthsXL, str);

	for (int i = 1; i < LCP_LEVEL; i++) {

		auto start_level = std::chrono::high_resolution_clock::now();
		size_t current_size = lcp::hash::cores_size;

		str->deepen(USE_MAP);

		auto stop_level = std::chrono::high_resolution_clock::now();
		distinct_core_counts[i] += lcp::hash::cores_size - current_size;
		durations[i] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(stop_level - start_level).count());
		if (str->cores != nullptr) {
			core_counts[i] += str->cores->size();
		}

		analyze(i, contiguous_counts, distances, distancesXL, lengths, lengthsXL, str);
	}

	std::cout << "Length of the processed sequence: " << format_int(sequence.size()) << std::endl;

	delete str;
	sequence.clear();
}

/**
 * @brief The entry point of the program.
 *
 * The main function coordinates the entire genomic data analysis process. It
 * initializes necessary data structures, reads input genomic sequences, and
 * calls specific functions to perform analysis at multiple levels. The function
 * also handles file operations, including reading from and writing to files, and
 * ensures proper flow and error handling throughout the execution.
 *
 * It performs sequence reading, individual and multi-level analysis, and finally
 * aggregates and summarizes the results before concluding the execution.
 *
 * Usage: <ExecutableName> <InputFile.fasta>
 */
int main(int argc, char **argv) {

	if (argc < 2) {
		std::cerr << "Wrong format: " << argv[0] << " [infile] " << std::endl;
		return -1;
	}

	std::ifstream input(argv[1]);
	if (!input.good()) {
		std::cerr << "Error opening: " << argv[1] << " . You have failed." << std::endl;
		return -1;
	}

	// variables
	std::string line;

	std::fstream genome;
	genome.open(argv[1], std::ios::in);

	int contiguous_counts[LCP_LEVEL] = {0};
	int core_counts[LCP_LEVEL] = {0};
	int distinct_core_counts[LCP_LEVEL] = {0};
	int distances[LCP_LEVEL][DISTANCE_LENGTH] = {0};
	std::vector<std::vector<int>> distancesXL(LCP_LEVEL);
	int lengths[LCP_LEVEL][DISTANCE_LENGTH] = {0};
	std::vector<std::vector<int>> lengthsXL(LCP_LEVEL);
	std::vector<std::chrono::milliseconds> durations(LCP_LEVEL);
	size_t genome_size = 0;

	// read file
	if (genome.is_open()) {

		std::string sequence, id;
		sequence.reserve(250000000);

		// initializing coefficients of the alphabet and hash tables
		lcp::encoding::init();

		if (USE_MAP) {
			lcp::hash::init(4000, 536870911);

			std::cout << "str_map.capacity at the begining: " << format_int(lcp::hash::str_map.max_load_factor() * lcp::hash::str_map.bucket_count()) << std::endl;
		}

		std::cout << "Program begins" << std::endl;

		while (getline(genome, line)) {

			if (line[0] == '>') {

				// process previous chromosome before moving into new one
				if (sequence.size() != 0) {
					genome_size += sequence.size();
					process(sequence, durations, contiguous_counts, core_counts, distinct_core_counts, distances, distancesXL, lengths, lengthsXL);
				}

				id = line.substr(1);
				std::cout << "Processing started for " << id << std::endl;
				continue;

			} else if (line[0] != '>') {
				sequence += line;
			}
		}

		if (sequence.size() != 0) {
			genome_size += sequence.size();
			process(sequence, durations, contiguous_counts, core_counts, distinct_core_counts, distances, distancesXL, lengths, lengthsXL);
		}

		genome.close();
	}

	std::string sep = " & ";
	double previous, current;

	std::cout << "LCP level";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << i + 1;
	}
	std::cout << std::endl;

	// Total Cores
	std::cout << "Total Cores";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_int(core_counts[i]);
	}
	std::cout << std::endl;

	// Contiguous Cores
	std::cout << "Contiguous Cores";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_int(contiguous_counts[i]);
	}
	std::cout << std::endl;

	// Distinct Cores
	std::cout << "Unique Cores " << (USE_MAP ? "(Table)" : "(Hash)");
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_int(distinct_core_counts[i]);
	}
	std::cout << std::endl;

	// Execution Time
	std::cout << "Exec. Time (sec) " << (USE_MAP ? "(Table)" : "(Hash)");
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_double(((double)durations[i].count()) / 1000);
	}
	std::cout << std::endl;

	// Mean Core Distances
	std::cout << "Avg. Dist.";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_double(mean(distances[i], distancesXL[i]));
	}
	std::cout << std::endl;

	// Std Dev of Distances
	std::cout << "StdDev Dist.";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_double(stdev(distances[i], distancesXL[i]));
	}
	std::cout << std::endl;

	// Mean Core Length
	std::cout << "Avg. Len.";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_double(mean(lengths[i], lengthsXL[i]));
	}
	std::cout << std::endl;

	// Std Dev of Lengths
	std::cout << "StdDev Len.";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_double(stdev(lengths[i], lengthsXL[i]));
	}
	std::cout << std::endl;

	// Decrease in Total Counts
	previous = genome_size;
	std::cout << "Decr. Core Cnt.";
	for (int i = 0; i < LCP_LEVEL; i++) {
		std::cout << sep << format_double(static_cast<double>(core_counts[i]) / previous);
		previous = static_cast<double>(core_counts[i]);
	}
	std::cout << std::endl;

	// Increase in Mean Lengths
	previous = 1;
	std::cout << "Incr. Avg. Len.";
	for (int i = 0; i < LCP_LEVEL; i++) {
		current = mean(lengths[i], lengthsXL[i]);
		std::cout << sep << format_double(current / previous);
		previous = current;
	}
	std::cout << std::endl;

	// Increase in Mean Distances
	previous = 1;
	std::cout << "Incr. Avg. Dist.";
	for (int i = 0; i < LCP_LEVEL; i++) {
		current = mean(distances[i], distancesXL[i]);
		std::cout << sep << format_double(current / previous);
		previous = current;
	}
	std::cout << std::endl;

	if (USE_MAP) {
		lcp::hash::summary();

		std::cout << "ID: " << lcp::hash::next_id << std::endl;
	}

	return 0;
};
