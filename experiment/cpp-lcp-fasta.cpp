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
#include "../string.cpp"
#include "helper.cpp"


/**
 * @brief Performs analysis of genomic data at a specified level.
 *
 * This function analyzes genomic data, specifically focusing on overlapping
 * counts, distances, and lengths at a given level of analysis. It iterates
 * through genomic cores, calculates relevant statistics, and stores them in
 * provided arrays and vectors. It supports analysis at different levels,
 * allowing for multi-layered examination of genomic sequences.
 *
 * @param level The level of analysis (used in multi-level processing).
 * @param overlapping_counts Array to store counts of overlapping genomic segments.
 * @param all_distances Multidimensional array to store distances between genomic features.
 * @param all_distances_pos Multidimensional array to store position-based distances.
 * @param all_lengths Multidimensional array to store lengths of genomic segments.
 * @param all_larger_distances_vec Vector to store larger distances.
 * @param all_larger_distances_pos_vec Vector to store larger position-based distances.
 * @param all_larger_lengths_vec Vector to store larger lengths.
 * @param str Pointer to the genomic string being analyzed.
 * @param chrom_index Index of the current chromosome being processed.
 */
void analyze( int level, int (&overlapping_counts)[LCP_LEVEL], int (&all_distances)[LCP_LEVEL][2*DISTANCE_LENGTH], int (&all_distances_pos)[LCP_LEVEL][DISTANCE_LENGTH], int (&all_lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, lcp::string *str, int chrom_index ) {
    
    for ( std::deque<lcp::core*>::iterator it = str->cores.begin() + 1; it < str->cores.end(); it++ ) {

        if ( (*it)->start < (*(it-1))->end ) {
            overlapping_counts[level] += 1;
        }
        if ( 0 <= (*it)->start - (*(it-1))->end + DISTANCE_LENGTH && (*it)->start - (*(it-1))->end < DISTANCE_LENGTH ) {
            all_distances[level][(*it)->start - (*(it-1))->end + DISTANCE_LENGTH]++;
        } else {
            all_larger_distances_vec[level].push_back((*it)->start - (*(it-1))->end);
        }

        if ( 0 <= (*it)->start - (*(it-1))->start && (*it)->start - (*(it-1))->start < DISTANCE_LENGTH ) {
            all_distances_pos[level][(*it)->start - (*(it-1))->start]++;
        } else {
            all_larger_distances_pos_vec[level].push_back( (*it)->start - (*(it-1))->start );
        }
        
        if ( 0 <= (*it)->end - (*it)->start && (*it)->end - (*it)->start < DISTANCE_LENGTH ) {
            all_lengths[level][(*it)->end - (*it)->start] += 1;
        } else {
            all_larger_lengths_vec[level].push_back( (*it)->end - (*it)->start );
        }
    }
};


/**
 * @brief Performs analysis of genomic data at a specified level.
 *
 * This function analyzes genomic data, specifically focusing on overlapping
 * counts, distances, and lengths at a given level of analysis. It iterates
 * through genomic cores, calculates relevant statistics, and stores them in
 * provided arrays and vectors. It supports analysis at different levels,
 * allowing for multi-layered examination of genomic sequences.
 * 
 * The difference between the other analyze function is that this function is targetting
 * first level analyses of the given string.
 *
 * @param overlapping_counts Array to store counts of overlapping genomic segments.
 * @param all_distances Multidimensional array to store distances between genomic features.
 * @param all_distances_pos Multidimensional array to store position-based distances.
 * @param all_lengths Multidimensional array to store lengths of genomic segments.
 * @param all_larger_distances_vec Vector to store larger distances.
 * @param all_larger_distances_pos_vec Vector to store larger position-based distances.
 * @param all_larger_lengths_vec Vector to store larger lengths.
 * @param str Pointer to the genomic string being analyzed.
 * @param chrom_index Index of the current chromosome being processed.
 */
void analyze( int (&overlapping_counts)[LCP_LEVEL], int (&all_distances)[LCP_LEVEL][2*DISTANCE_LENGTH], int (&all_distances_pos)[LCP_LEVEL][DISTANCE_LENGTH], int (&all_lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, lcp::string *str, int chrom_index ) {
    
    int level = 0;
    
    for ( std::deque<lcp::base_core*>::iterator it = str->base_cores.begin() + 1; it < str->base_cores.end(); it++ ) {

        if ( (*it)->start < (*(it-1))->end ) {
            overlapping_counts[level] += 1;
        }
        if ( 0 <= (*it)->start - (*(it-1))->end + DISTANCE_LENGTH && (*it)->start - (*(it-1))->end < DISTANCE_LENGTH ) {
            all_distances[level][(*it)->start - (*(it-1))->end + DISTANCE_LENGTH]++;
        } else {
            all_larger_distances_vec[level].push_back((*it)->start - (*(it-1))->end);
        }

        if ( 0 <= (*it)->start - (*(it-1))->start && (*it)->start - (*(it-1))->start < DISTANCE_LENGTH ) {
            all_distances_pos[level][(*it)->start - (*(it-1))->start]++;
        } else {
            all_larger_distances_pos_vec[level].push_back( (*it)->start - (*(it-1))->start );
        }
        
        if ( 0 <= (*it)->end - (*it)->start && (*it)->end - (*it)->start < DISTANCE_LENGTH ) {
            all_lengths[level][(*it)->end - (*it)->start] += 1;
        } else {
            all_larger_lengths_vec[level].push_back( (*it)->end - (*it)->start );
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

    int chrom_index = 0;
    int all_overlapping_counts[LCP_LEVEL] = {0};
    int all_core_count[LCP_LEVEL] = {0};
    int all_distances[LCP_LEVEL][2*DISTANCE_LENGTH] = {0};  // in order to store overlapping cores as well (as distance will be negative)
    int all_distances_pos[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    int all_lengths[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    std::vector<std::chrono::milliseconds> all_durations(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_distances_vec(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_distances_pos_vec(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_lengths_vec(LCP_LEVEL);

	// read file
    if ( genome.is_open() ) {  

        std::string sequence, id;
        sequence.reserve(250000000);
                
        std::cout << "Program begins" << std::endl;

        while (getline(genome, line)) {

            if (line[0] == '>') {

                chrom_index++;

                // process previous chromosome before moving into new one
                if (sequence.size() != 0) {
                    auto start = std::chrono::high_resolution_clock::now();
                
                    lcp::string *str = new lcp::string(sequence);
                    
                    auto extraction_end = std::chrono::high_resolution_clock::now();
                    all_durations[0] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(extraction_end - start).count());
                    all_core_count[0] += str->base_cores.size();
                    
                    // Base cores are stored in level 0 instead of cores. Hence, need seperation
                    analyze(all_overlapping_counts, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, str, chrom_index);
                    
                    for ( int i = 1; i < LCP_LEVEL; i++ ) {

                        auto start_level = std::chrono::high_resolution_clock::now();

                        str->deepen();
                        
                        auto stop_level = std::chrono::high_resolution_clock::now();
                        all_durations[i] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(stop_level - start_level).count());
                        all_core_count[i] += str->cores.size();

                        analyze(i, all_overlapping_counts, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, str, chrom_index);
                    }

                    std::cout << "Processing is done for " << id << std::endl;
                    std::cout << "Length of the processed sequence: " << sequence.size() << std::endl;
                    std::cout << std::endl;
                    
                    delete str;

                    sequence.clear();
                }

                if ( chrom_index > 23 ) {
                    break;
                }
                
                id = line.substr(1);
                std::cout << "Processing started for " << id << std::endl;
                continue;
                    
            }
            else if (line[0] != '>'){
                sequence += line;
            }
        }

        summaryLCP( all_overlapping_counts, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, all_durations, all_core_count);

        genome.close();
    }

    // std::ofstream outfile ( "detailed_summary.lcp.fasta.txt" );
    
    // print2file( all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, outfile );

    // outfile.close();

    return 0;
};
