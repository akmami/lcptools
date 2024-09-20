/**
 * @file    cpp-lcp-fasta.cpp
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
#include <chrono>
#include "lps.h"
#include "../utils/helper.cpp"


#define STRING_SIZE         250000000
#define MAX_CORE_COUNT      300000000


void process(std::string& sequence, std::vector<std::chrono::milliseconds>& durations, int (&core_counts)[LCP_LEVEL], int (&distinct_core_counts)[LCP_LEVEL], /**double (&sizes)[LCP_LEVEL],*/ std::vector<uint>& core_count, std::vector<lcp::lps*>& strs ) {
    
    auto start = std::chrono::high_resolution_clock::now();
    size_t initial_size = lcp::str_map.size();
    lcp::lps *str = new lcp::lps(sequence);
    
    auto extraction_end = std::chrono::high_resolution_clock::now();
    core_counts[0] += str->cores->size();
    str->update_core_count(core_count);
    distinct_core_counts[0] += lcp::str_map.size() - initial_size;
    durations[0] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(extraction_end - start).count());
    // sizes[0] += str->memsize();

    for ( int i = 1; i < LCP_LEVEL; i++ ) {

        auto start_level = std::chrono::high_resolution_clock::now();
        size_t current_size =  lcp::core_map.size();

        str->deepen();
        
        auto stop_level = std::chrono::high_resolution_clock::now();
        str->update_core_count(core_count);
        core_counts[i] += str->cores->size();
        distinct_core_counts[i] += lcp::core_map.size() - current_size;
        durations[i] += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(stop_level - start_level).count());
        // sizes[i] += str->memsize();
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
    // double sizes[LCP_LEVEL] = {0};
    int core_counts[LCP_LEVEL] = {0};
    int distinct_core_counts[LCP_LEVEL] = {0};
    std::vector<uint> core_count;
    lcp::init_core_counts( core_count, MAX_CORE_COUNT ); // no overfow handled yet
    std::vector<uint> subcores;
    
    // read file
    if ( genome.is_open() ) {  
        
        std::string sequence, id;
        sequence.reserve(STRING_SIZE);

        // initializing coefficients of the alphabet and hash tables
        lcp::init_coefficients();
        lcp::init_hashing(4000, MAX_CORE_COUNT);
                
        std::cout << "Program begins" << std::endl;

        std::cout << "str_map.capacity at the begining: " << format_int( lcp::str_map.max_load_factor() * lcp::str_map.bucket_count() ) << std::endl;
        std::cout << "core_map.capacity at the begining: " << format_int( lcp::core_map.max_load_factor() * lcp::core_map.bucket_count() ) << std::endl;
        
        while ( getline(genome, line) ) {

            if (line[0] == '>') {

                // process previous chromosome before moving into new one
                if (sequence.size() != 0) {
                    process( sequence, durations, core_counts, distinct_core_counts, /**sizes,*/ core_count, strs );
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
            process( sequence, durations, core_counts, distinct_core_counts, /**sizes,*/ core_count, strs );
        }

        genome.close();
    }

    std::cout << std::endl;

    std::cout << "str_map.capacity at the end: " << format_int(lcp::str_map.max_load_factor() * lcp::str_map.bucket_count() ) << std::endl;
    std::cout << "str_map.size at the end: " << format_int( lcp::str_map.size() ) << std::endl;
    
    std::cout << std::endl;

    std::cout << "core_map.capacity at the end: " << format_int( lcp::core_map.max_load_factor() * lcp::core_map.bucket_count() ) << std::endl;
    std::cout << "core_map.size at the end: " << format_int( lcp::core_map.size() ) << std::endl;

    std::cout << std::endl;

    std::string sep = " & ";

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

    // Distinct Cores
    std::cout << "Distinct Cores";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_int(distinct_core_counts[i]);
    }
    std::cout << std::endl;

    // Execution Time
    std::cout << "Execution Time (sec)";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(((double) durations[i].count()) / 1000);
    }
    std::cout << std::endl;

    // // Total Sizes
    // std::cout << "Total Sizes(GB)";
    // for (int i = 0; i < LCP_LEVEL; i++) {
    //     std::cout << sep << format_double(sizes[i] / (1024.0 * 1024.0 * 1024.0));
    // }
    // std::cout << std::endl;
    
    uint core_count_sum = 0;

    for ( size_t i = 0; i < MAX_CORE_COUNT; i++ ) {
        core_count_sum += core_count[i]; 
    } 

    std::cout << std::endl;
    
    std::cout << "ID: " << format_int(lcp::next_id) << std::endl;
    std::cout << "Total number of cores: " << core_count_sum << std::endl;
    
    std::cout << std::endl;

    std::vector<unsigned short> lcp_levels;

    lcp::set_lcp_levels( lcp_levels );

    size_t level4count = 0;
    for ( size_t i = 0; i < lcp_levels.size(); i++ ) {
        if ( lcp_levels[i] == 3 ) {
            level4count += core_count[i]; 
        }
    } 
    std::cout << "Total number of cores at level 4: " << level4count << std::endl;

    lcp::init_reverse();
    
    // get top level LCP cores
    std::vector<uint> labels;
    for( std::vector<lcp::lps*>::iterator it = strs.begin(); it != strs.end(); it++ ) {
        (*it)->get_labels( labels );
    }
    
    // get sub-level LCP cores
    std::vector<uint> sub_labels, sub_count;
    lcp::get_sublevel_labels( labels, core_count, sub_labels, sub_count );

    size_t sublevel_count_sum = 0;
    for( std::vector<uint>::iterator it = sub_count.begin(); it != sub_count.end(); it++ ) {
        sublevel_count_sum += (*it);
    }
    
    std::cout << "Total number of distinct cores at top LCP level: " << format_int(labels.size()) << std::endl;
    std::cout << "Total number of sublevel core count (found): " << format_int(sublevel_count_sum) << std::endl;
    std::cout << "Total number of distinct sublevel cores (found): " << format_int(sub_labels.size()) << std::endl;
    
    std::cout << std::endl;
    
    // calculate distinct core number
    core_count_sum = 0;
    for ( size_t i = 0; i < MAX_CORE_COUNT; i++ ) {
        core_count_sum += core_count[i]; 
    }

    std::cout << "Total number of distinct cores (it should match with initial sum): " << format_int(core_count_sum) << std::endl;

    // cleanup
    for( std::vector<lcp::lps*>::iterator it = strs.begin(); it != strs.end(); it++ ) {
        delete (*it);
    }
    strs.clear();

    return 0;
};
