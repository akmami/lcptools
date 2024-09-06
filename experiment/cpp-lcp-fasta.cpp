/**
 * @file    cpp-lcp-fasta.cpp
 * @brief   Analysis and Processing of Genomic Data
 *
 * This program is designed for in-depth analysis of genomic sequences. It reads
 * genomic data, processes it through multiple levels of analysis (defined by
 * LCP_LEVEL), and computes various statistics such as overlapping counts, distances,
 * and lengths of genomic sequences.
 *
 * The analysis is detailed and involves different stages, including the computation
 * of overlapping regions, distances between certain genomic features, and lengths of
 * various genomic segments. The results are then summarized to provide insights into
 * the genomic data structure.
 *
 * The program leverages a series of custom functions and data structures, operating
 * on large volumes of data with efficiency and accuracy in focus.
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include "lps.h"
#include "../utils/helper.cpp"


/**
 * @brief Performs analysis of genomic data at a specified level.
 *
 * This function analyzes genomic data, specifically focusing on overlapping
 * counts, distances, and lengths at a given level of analysis. It iterates
 * through LCP cores, calculates relevant statistics, and stores them in
 * provided arrays and vectors. It supports analysis at different levels,
 * allowing for multi-layered (LCP level) examination of genomic sequences.
 *
 * @param level The level of analysis (used in multi-level processing).
 * @param overlapping_counts Array to store counts of overlapping genomic segments.
 * @param distances Multidimensional array to store position-based distances.
 * @param distancesXL Vector to store larger position-based distances.
 * @param lengths Multidimensional array to store lengths of genomic segments.
 * @param lengthsXL Vector to store larger lengths.
 * @param str Pointer to the genomic string being analyzed.
 */
void analyze( int level, int (&overlapping_counts)[LCP_LEVEL], int (&distances)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &distancesXL, int (&lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &lengthsXL, lcp::lps *str ) {
    
    for ( std::vector<lcp::core*>::iterator it = str->cores.begin() + 1; it < str->cores.end(); it++ ) {

        if ( (*it)->start < (*(it-1))->end ) {
            overlapping_counts[level] += 1;
        }

        if ( (*it)->start < DISTANCE_LENGTH + (*(it-1))->start ) {
            distances[level][(*it)->start - (*(it-1))->start]++;
        } else {
            distancesXL[level].push_back( (*it)->start - (*(it-1))->start );
        }
        
        if ( (*it)->end < DISTANCE_LENGTH + (*it)->start ) {
            lengths[level][(*it)->end - (*it)->start] += 1;
        } else {
            lengthsXL[level].push_back( (*it)->end - (*it)->start );
        }
    }
};


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

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string line;

    std::fstream genome;
    genome.open(argv[1], std::ios::in);

    int overlapping_counts[LCP_LEVEL] = {0};
    int core_counts[LCP_LEVEL] = {0};
    int distances[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    std::vector<std::vector<int>> distancesXL(LCP_LEVEL);
    int lengths[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    std::vector<std::vector<int>> lengthsXL(LCP_LEVEL);
    std::vector<std::chrono::milliseconds> durations(LCP_LEVEL);

	// read file
    if ( genome.is_open() ) {  

        std::string sequence, id;
        sequence.reserve(250000000);
                
        std::cout << "Program begins" << std::endl;

        while (getline(genome, line)) {

            if (line[0] == '>') {

                // process previous chromosome before moving into new one
                if (sequence.size() != 0) {
                    auto start = std::chrono::high_resolution_clock::now();
                
                    lcp::lps *str = new lcp::lps(sequence);
                    
                    auto extraction_end = std::chrono::high_resolution_clock::now();
                    durations[0] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(extraction_end - start).count());
                    core_counts[0] += str->cores.size();
                    
                    // Base cores are stored in level 0 instead of cores. Hence, need seperation
                    analyze(0, overlapping_counts, distances, distancesXL, lengths, lengthsXL, str);
                    
                    for ( int i = 1; i < LCP_LEVEL; i++ ) {

                        auto start_level = std::chrono::high_resolution_clock::now();

                        str->deepen();
                        
                        auto stop_level = std::chrono::high_resolution_clock::now();
                        durations[i] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(stop_level - start_level).count());
                        core_counts[i] += str->cores.size();

                        analyze(i, overlapping_counts, distances, distancesXL, lengths, lengthsXL, str);
                    }

                    std::cout << "Processing is done for " << id << std::endl;
                    std::cout << "Length of the processed sequence: " << sequence.size() << std::endl;
                    std::cout << std::endl;
                    
                    delete str;

                    sequence.clear();
                }
                
                id = line.substr(1);
                std::cout << "Processing started for " << id << std::endl;
                continue;
                    
            }
            else if (line[0] != '>'){
                sequence += line;
            }
        }

        summaryLCP( overlapping_counts, distances, distancesXL, lengths, lengthsXL, durations, core_counts);

        genome.close();
    }

    // std::ofstream outfile ( "detailed_summary.lcp.fasta.txt" );
    
    // print2file( distances, lengths, distancesXL, lengthsXL, outfile );

    // outfile.close();

    return 0;
};
