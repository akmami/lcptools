/**
 * @file    fastq-to-fasta.cpp
 * @brief   Genomic Data Preprocessing Tool
 *
 * This program processes genomic data files in a specific format. It expects
 * two arguments: an input file and an output file, both expected to be in
 * gzip-compressed format. The program performs the following operations:
 *
 * 1. Validates the input and output file paths provided as command-line arguments.
 * 2. Reads data from the gzip-compressed input file using a GzFile object.
 *    - It assumes a specific format for the data, where each record consists of
 *      multiple lines, including sequence information and quality scores.
 * 3. Processes each record by:
 *    - Replacing the first character of the sequence identifier line ('@') with '>'.
 *    - Writing the modified sequence identifier and the sequence itself to the output file.
 *    - Skipping the lines that follow the sequence line, usually representing quality scores.
 * 4. Handles any file reading errors and terminates the process if an error occurs.
 *
 * Note: The constant BUFFERSIZE is defined in the GzFile.hpp file, specifying
 * the size of the buffer used for reading from the input file.
 *
 * Usage: <ExecutableName> <InputFile.gz> <OutputFile.gz>
 */

#include <iostream>
#include <fstream>
#include "GzFile.hpp"


int main(int argc, char* argv[]) {
    
    // Validate arguments
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

    char buffer[BUFFERSIZE];

    while (true) {
        
        if ( infile.gets(buffer, sizeof(buffer)) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        outfile.printf(">%s", buffer + 1);        // Replace '@' with '>'

        infile.gets(buffer, sizeof(buffer));
        outfile.printf("%s", buffer);             // Write the sequence
        
        infile.gets(buffer, sizeof(buffer));        // Skip the '+' line
        infile.gets(buffer, sizeof(buffer));        // Skip the quality line
    }

    return 0;
}
