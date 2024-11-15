/**
 * @file    lcp-fasta.cpp
 * @brief   Analysis and Processing of Genomic Data
 *
 * This program is designed for in-depth analysis of genomic sequences. It reads
 * genomic data, processes it through multiple levels of analysis (defined by
 * LCP_LEVEL), and computes memory usage of the program alogn with execution times.
 *
 * The program leverages a series of custom functions and data structures, operating
 * on large volumes of data with efficiency and accuracy in focus.
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <chrono>
#include "lps.h"
#include "helper.cpp"


#define STRING_SIZE         250000000
#define MAX_CORE_COUNT      536870911
#define COMPUTE_SIZES       false
#define USE_MAP             false


/**
 * @brief Processes genomic sequence data, tracks execution time, and analyzes 
 * various statistics.
 *
 * This function processes a given genomic sequence by performing multiple levels 
 * of LCP (Locally Consistent Parsing) analysis. It tracks the execution time for 
 * each level of processing, updates various statistical arrays, and manages LCP core
 * extraction and deepening across specified levels.
 *
 * @param sequence The genomic sequence (string) to be analyzed.
 * @param durations A vector storing the durations (in milliseconds) of each level's 
 *                  processing time.
 * @param total_core_counts An array storing the number of LCP cores found at each level.
 * @param sizes An array storing sizes (bytes) of LCP cores found at each level.
 * @param strs An vector of lps objects obtained after processing given sequences.
 */
void process(std::string& sequence, std::vector<std::chrono::milliseconds>& durations, size_t (&total_core_counts)[LCP_LEVEL], double (&sizes)[LCP_LEVEL], std::vector<lcp::lps*>& strs ) {
    
    auto start = std::chrono::high_resolution_clock::now();
    lcp::lps *str = new lcp::lps(sequence, USE_MAP);
    
    auto extraction_end = std::chrono::high_resolution_clock::now();
    total_core_counts[0] += str->size();
   
    durations[0] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(extraction_end - start).count());
    
    if( COMPUTE_SIZES ) {
        sizes[0] += str->memsize();
    }

    for ( int i = 1; i < LCP_LEVEL; i++ ) {

        auto start_level = std::chrono::high_resolution_clock::now();
        
        str->deepen(USE_MAP);
        
        auto stop_level = std::chrono::high_resolution_clock::now();
        total_core_counts[i] += str->size();
        
        durations[i] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(stop_level - start_level).count());
        
        if( COMPUTE_SIZES ) {
            sizes[i] += str->memsize();
        }
    }

    std::cout << "Length of the processed sequence: " << format_int(sequence.size()) << std::endl;
    
    strs.push_back( str );
    sequence.clear();

    str = nullptr;
}

/**
 * @brief The entry point of the program.
 *
 * The main function coordinates the entire genomic data analysis process. It 
 * initializes necessary data structures, reads input genomic sequences, and 
 * stores all lcp cores in vector to measure memory usage.
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

    std::vector<lcp::lps*> strs;
    std::vector<std::chrono::milliseconds> durations(LCP_LEVEL);
    double sizes[LCP_LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0};
    size_t total_core_counts[LCP_LEVEL] = {0, 0, 0, 0, 0, 0, 0, 0};
    
    // read file
    if ( genome.is_open() ) {  
        
        std::string sequence, id;
        sequence.reserve(STRING_SIZE);

        // initializing coefficients of the alphabet and hash tables
        lcp::encoding::init();
        if ( USE_MAP ) {
            lcp::hash::init(4000, MAX_CORE_COUNT);
        }

        std::cout << "Program begins" << std::endl;

        while ( getline(genome, line) ) {

            if (line[0] == '>') {

                // process previous chromosome before moving into new one
                if (sequence.size() != 0) {
                    process( sequence, durations, total_core_counts, sizes, strs );
                }

                id = line.substr(1);
                std::cout << "Processing started for " << id << std::endl;
                continue;
                    
            }
            else if (line[0] != '>'){
                sequence += line;
            }
        }

        if (sequence.size() != 0) {
            process( sequence, durations, total_core_counts, sizes, strs );
        }

        genome.close();
    }

    std::cout << std::endl;

    std::string sep = " & ";

    std::cout << "LCP level";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << i + 1;
    }
    std::cout << std::endl;

    // Total Cores
    std::cout << "Total # of Cores";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_int(total_core_counts[i]);
    }
    std::cout << std::endl;

    // Execution Time
    std::cout << "Exec. Time (sec) " << ( USE_MAP ? "(Table)" : "(Hash)" );
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(((double) durations[i].count()) / 1000);
    }
    std::cout << std::endl;
    

    // Total Sizes
    if ( COMPUTE_SIZES ) {
        std::cout << "Total Size (GB)**";
        for (int i = 0; i < LCP_LEVEL; i++) {
            std::cout << sep << format_double(sizes[i] / (1024.0 * 1024.0 * 1024.0));
        }
        std::cout << std::endl << std::endl;
    }
    

    if ( USE_MAP ) {
        std::cout << "ID: " << format_int(lcp::hash::next_id) << std::endl;

        std::cout << std::endl;

        lcp::hash::summary();
    }

    // cleanup
    for( std::vector<lcp::lps*>::iterator it = strs.begin(); it != strs.end(); it++ ) {
        delete (*it);
    }
    strs.clear();

    return 0;
};
