/**
 * @file    cpp-lcp-fastq-error.cpp
 * @brief   Genomic Sequence Analysis and Comparison Tool
 *
 * This program is designed for the analysis and comparison of genomic sequences.
 * It reads genomic data, processes the sequences, and compares them at a specified
 * level of detail (LCP level). The comparison involves counting matches between
 * genomic cores of two sequences based on certain criteria, such as the maximum
 * allowable distance difference.
 *
 * The program uses custom data structures and algorithms to efficiently process
 * and compare large genomic sequences. It is capable of handling input from gzip-compressed
 * files and supports detailed analysis with an emphasis on accuracy and performance.
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include "../string.cpp"
#include "process.cpp"
#include "GzFile.hpp"


#define READ_COUNT              1000
#define MAX_DISTANCE_DIFF       30
#define RESULT_ARRAY_SIZE       5000


/**
 * @brief Compares cores between two genomic sequences and counts matches.
 *
 * This function analyzes two genomic sequences, represented by lcp::string objects,
 * and compares their cores. A match is counted if the cores are equal and the start
 * positions are within a specified distance difference. The function keeps track of
 * the last matched position in the second sequence to optimize subsequent comparisons.
 *
 * @param seq1 Pointer to the first lcp::string representing a genomic sequence.
 * @param seq2 Pointer to the second lcp::string representing a genomic sequence.
 * @param match Reference to an integer that stores the count of matches found.
 */
void analyze(lcp::string* seq1, lcp::string* seq2, int& match, int& total ) {
    
    std::deque<lcp::core *>::iterator it1;
    std::deque<lcp::core *>::iterator lastMatchIt = seq2->cores.begin() - 1;

    for(std::deque<lcp::core *>::iterator it1 = seq1->cores.begin(); it1 != seq1->cores.end(); it1++ ) {
        for(std::deque<lcp::core *>::iterator it2 = lastMatchIt + 1; it2 != seq2->cores.end(); it2++ ) {
            if ( *(*it1) == *(*it2) && ( (*it1)->start < (*it2)->start ? (*it2)->start - (*it1)->start : (*it1)->start - (*it2)->start) < MAX_DISTANCE_DIFF ) {
                match++;
                lastMatchIt = it2;
                break;
            }
        }

        total++;
    }
};


/**
 * @brief The entry point of the program for genomic sequence comparison.
 *
 * The main function orchestrates the process of reading, processing, and comparing
 * genomic sequences. It initializes necessary data structures, reads sequences from
 * a gzip-compressed input file, and performs comparison at a specified LCP level.
 * The function outputs the matches found and maintains a count of the number of reads
 * processed. It ensures proper flow and error handling throughout the execution.
 *
 * Usage: <ExecutableName> <InputFile.fastq.gz> <LCP_Level>
 */
int main(int argc, char **argv) {

    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [lcp-level]" << std::endl;
        return -1;  
    }

    // validate input file
    GzFile infile( argv[1], "rb");
    if (!infile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    int lcp_level = atoi(argv[2]);
    
    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string str1, str2;
    int read_count = 0;
    int match, total;
    
	// read file
    std::cout << "Match:\tTotal" << std::endl;

    char buffer[BUFFERSIZE];
    std::istringstream iss;

    uint results[RESULT_ARRAY_SIZE] = {0};
    bool isFirstValid, isSecondValid;

    while ( true ) {

        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }
        
        infile.gets(buffer, BUFFERSIZE);

        isFirstValid = process(buffer);

        iss.clear();
        iss.str(buffer);
        iss >> str1 >> str1 >> str1 >> str1 >> str1 >> str1 >> str1;

        infile.gets(buffer, BUFFERSIZE);
        
        isSecondValid = process(buffer);

        iss.clear();
        iss.str(buffer);
        iss >> str2 >> str2 >> str2 >> str2 >> str2 >> str2 >> str2;
        
        infile.gets(buffer, BUFFERSIZE);

        if ( ! isFirstValid || ! isSecondValid ) {
            continue;
        }
        
        // Process sequences
        lcp::string *str_original = new lcp::string(str1);
        lcp::string *str_error = new lcp::string(str2);

        for ( int i = 1; i < lcp_level; i++ ) {
            str_original->deepen();
            str_error->deepen();
        }

        match = 0;
        total = 0;
        analyze( str_original, str_error, match, total );

        // std::cout << str_original << std::endl;
        // std::cout << str_error << std::endl;

        delete str_original;
        delete str_error;

        std::cout << match << ":\t\t" << total << std::endl;

        results[match]++;

        read_count++;

        if ( read_count == READ_COUNT ){
            break;
        }
    }

    std::cout << "Processed reads: " << read_count << " lcp_level" << lcp_level << std::endl;

    for ( uint i = 0; i < RESULT_ARRAY_SIZE; i++ ) {
        if ( results[i] != 0 ) {
            std::cout << i << ": " << results[i] << ",\t";
        }
    }

    return 0;
};
