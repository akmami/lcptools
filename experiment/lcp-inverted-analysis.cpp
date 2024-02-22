/**
 * @file    main.cpp
 * @brief   Construction and Analysis of Inverted Index from Genomic Data
 *
 * This program reads genomic sequence data from a gzip-compressed file, processes
 * each sequence to extract features, and constructs an inverted index. The index
 * maps feature identifiers (derived from sequences) to document (sequence) IDs,
 * facilitating efficient retrieval and analysis of sequences based on shared features.
 * The program supports specifying the level of detail for feature extraction through
 * the LCP (Longest Common Prefix) structure's deepening process. After constructing
 * the index, the program performs an analysis to summarize the index contents and
 * potentially identify significant patterns in the genomic data. The results are
 * outputted to a specified file.
 *
 * Usage:
 *     ./executableName inputFile.gz outputFile [lcp-level]
 */


#include <iostream>
#include <sstream>
#include "../utils/GzFile.hpp"
#include "InvertedIndex.hpp"
#include "../utils/process.cpp"
#include "../string.cpp"


// #define READCOUNT   10


/**
 * @brief Main function for reading and processing genomic data to build an inverted index.
 *
 * Validates command-line arguments for input and output files and an LCP level. Opens and
 * reads a gzip-compressed file containing genomic sequences, processes each sequence to
 * extract features at a specified LCP level, and inserts them into an inverted index.
 * After building the index, the function analyzes the index to provide insights into
 * the genomic data, such as identifying the most common features among the sequences.
 * The results of the construction and analysis of the inverted index are written to
 * an output file.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Returns 0 on successful execution and termination, 1 on file opening failures, 
 *         or -1 on incorrect usage (wrong number of arguments).
 */
int main(int argc, char **argv) {

    // validate arguments
    if (argc != 4) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [outfile] [lcp-level]" << std::endl;
        return -1;
    }

    // validate input file
    GzFile infile(argv[1], "rb");
    if (!infile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // validate output file
    std::ofstream outfile(argv[2]);
    if ( !outfile.good() ) {
        std::cerr << "Failed to open file: " << argv[2] << std::endl;
        return 1;
    }

    int lcp_level = atoi(argv[3]);

    // variables
    char buffer[BUFFERSIZE];
    std::string str;

    str.reserve(100000);
    
    doc_id_type id_index = 1;

    lcp::init_coefficients();
    InvertedIndex *inverted_index = new InvertedIndex();

    outfile << "Constructing inverted index..." << std::endl;

    while (true) {
        
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        infile.gets(buffer, BUFFERSIZE);    // get first sequence
        str.append(buffer);
        
        lcp::string *lcp = new lcp::string(str);
        lcp->deepen(lcp_level);

        inverted_index->insert(id_index, lcp);

        infile.gets(buffer, BUFFERSIZE);
        infile.gets(buffer, BUFFERSIZE);

        str.clear();
        id_index++;

        delete lcp;
    }

    inverted_index->analyze( outfile, lcp_level );

    outfile << "Inverted index structure constructed." << std::endl;
    outfile << "Analyzing inverted index." << std::endl;
    outfile << "Total number of reads being processed: " << id_index - 1 << std::endl;

    outfile.close();

    delete inverted_index;

    return 0;
}
