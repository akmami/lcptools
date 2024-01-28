/**
 * @file    extract-chrom.cpp
 * @brief   Chromosome-based Filtering of Genomic Data
 *
 * This program processes a gzip-compressed genomic data file and filters the content
 * based on a specified chromosome index. It reads the input file line by line, searches
 * for a specific chromosome ID, and upon finding a match, it writes the ID and associated
 * sequence data to a gzip-compressed output file.
 *
 * The program is designed to handle large genomic datasets efficiently and is capable
 * of reading and writing gzip-compressed files, making it suitable for high-throughput
 * genomic data processing.
 */


#include <iostream>
#include <fstream>
#include "GzFile.hpp"


/**
 * @brief The entry point of the program for chromosome-based filtering.
 *
 * The main function manages the entire process of reading a gzip-compressed input file,
 * filtering the genomic data based on a specified chromosome index, and writing the
 * filtered data to a gzip-compressed output file. The program validates command-line
 * arguments, handles file operations, performs the filtering based on the chromosome ID
 * specified, and ensures efficient and accurate processing.
 *
 * Usage: <ExecutableName> <InputFile.gz> <OutputFile.gz> <ChromosomeIndex>
 */
int main(int argc, char* argv[]) {
    
    // Validate arguments
    if (argc != 4) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [outfile] [chrom-index]" << std::endl;
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

    uint chrom = std::stoul( argv[3] );

    char buffer[BUFFERSIZE];

    while (true) {
        
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        std::string id(buffer);

        size_t firstUnderscore = id.find('_');
            
        if (chrom == std::stoul( id.substr(2, firstUnderscore - 2) ) ) {
            outfile.printf("%s", buffer);       // Write id
            
            infile.gets(buffer, BUFFERSIZE);
            outfile.printf("%s", buffer);       // Write the sequence

            infile.gets(buffer, BUFFERSIZE);    // Skip the '+' line
            outfile.printf("%s", buffer);

            infile.gets(buffer, BUFFERSIZE);    // Skip the quality line
            outfile.printf("%s", buffer);

            continue;
        } 

        infile.gets(buffer, BUFFERSIZE);

        infile.gets(buffer, BUFFERSIZE);

        infile.gets(buffer, BUFFERSIZE);
    }

    return 0;
}
