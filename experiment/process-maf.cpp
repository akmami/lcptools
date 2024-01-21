/**
 * @file    process-maf.cpp
 * @brief   MAF File Preprocessing for Genomic Sequences
 *
 * This program is tailored for processing MAF (Multiple Alignment Format) files,
 * particularly focusing on two types of reads:
 * 1. Original reads directly extracted from the genome.
 * 2. Simulated reads which include intentional errors to mimic sequencing variations.
 *
 * Each read comes with alignment data that maps to a reference sequence. In these
 * alignments, insertions and deletions are marked with '-' characters. The primary
 * function of this program is to process these reads by removing the '-' characters,
 * thereby normalizing the reads for subsequent analysis.
 *
 * The process involves:
 * - Validating and opening the input and output files, both expected to be in 
 *   gzip-compressed format.
 * - Iteratively reading sequences from the input file, where each sequence consists
 *   of several lines (including the sequence itself and potentially other metadata).
 * - Removing '-' characters from the sequences to correct for insertions and deletions.
 * - Writing the processed sequences to the output file.
 *
 * The program uses a buffer size defined by BUFFERSIZE for reading the input file.
 * It handles errors in file operations and ensures proper file format adherence.
 *
 * Usage: <ExecutableName> <InputFile.gz> <OutputFile.gz>
 */


#include <fstream>
#include <iostream>
#include "GzFile.hpp"


/**
 * @brief Processes a string by removing '-'.
 *
 * This function is designed to modify a given character array in place. It removes
 * all '-' characters from the array up to the first newline character '\n' or until
 * BUFFERSIZE is reached. The purpose is typically to clean up a string by eliminating
 * specific unwanted characters (e.g., in genomic data processing, '-' might represent
 * a deletion or a gap in an alignment that needs to be removed for further analysis).
 *
 * The function maintains the order of the remaining characters and ensures the processed
 * string is properly null-terminated.
 *
 * @param str A character array of size BUFFERSIZE. The array is modified in place; '-' 
 *        characters are removed, and the array is truncated at the first newline character.
 */
void process( char str[BUFFERSIZE] ) {
    
    size_t writeIndex = 0, readIndex = 0;

    for ( ; readIndex < BUFFERSIZE && str[readIndex] != '\n'; readIndex++) {
        if (str[readIndex] != '-') {
            str[writeIndex++] = str[readIndex];
        }
    }
    str[writeIndex++] = '\n';
    str[writeIndex] = '\0';
};


int main(int argc, char **argv) {

    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [outfile]" << std::endl;
        return -1;  
    }

    // validate input file
    GzFile infile( argv[1], "rb");
    if (!infile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // Open output file
    GzFile outfile( argv[2], "wb" );
    if ( !outfile ) {
        std::cerr << "Error creating outfile " << argv[2] << std::endl;
        return -1;
    }

    // variables
    char buffer[BUFFERSIZE];

	// read file
    while (true) {

        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }
        // line is 'a'
        outfile.printf("%s", buffer);

        // get raw sequence 1
        infile.gets(buffer, BUFFERSIZE);
        // remove '-' from raw sequence 1
        process(buffer);
        // write sequence 1
        outfile.printf("%s", buffer);

        // get raw sequence 2
        infile.gets(buffer, BUFFERSIZE);
        // remove '-' from raw sequence 2
        process(buffer);
        // write sequence 2
        outfile.printf("%s", buffer);
        
        // empty line
        infile.gets(buffer, BUFFERSIZE);
        outfile.printf("%s", buffer);
    }

    return 0;
};
