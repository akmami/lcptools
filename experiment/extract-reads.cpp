/**
 * @file    extract-reads.cpp
 * @brief   Extracts Reads from MAF and Outputs to FASTQ.gz Files
 *
 * This program processes genomic sequences contained in a specified input file,
 * presumably in a .maf format or similar structured format. For each read encountered,
 * it performs specified processing (e.g., simulation or cleaning) and then outputs
 * the original and processed reads into two separate gzip-compressed FASTQ files.
 * The program is designed to handle large-scale genomic data efficiently, leveraging
 * gzip compression for input and output to minimize disk space usage. It validates
 * input arguments, ensures safe file operations, and provides feedback on the number
 * of reads processed.
 *
 * Usage:
 *     ./extract-reads input.maf outfile1.fastq.gz outfile2.fastq.gz
 *
 * Where 'input.maf' is the path to the input file containing the reads, and
 * 'outfile1.fastq.gz' and 'outfile2.fastq.gz' are the paths for the output FASTQ files
 * for original and processed reads, respectively.
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include "../utils/GzFile.hpp"
#include "../utils/process.cpp"


#define READ_COUNT      10000


/**
 * @brief Main execution point for the read extraction and processing program.
 *
 * Validates command-line arguments to ensure proper usage, then proceeds to open the
 * specified input MAF file and two output FASTQ.gz files for processing. The program
 * reads sequences from the input file, applies a 'process' function to each sequence
 * (the specifics of which should be defined separately), and writes the original and
 * processed sequences to the respective output files. The program tracks the number
 * of reads processed and reports this upon completion. It is designed to efficiently
 * process and handle genomic data in a compressed format.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Returns 0 on successful completion, or a non-zero error code on failure.
 *
 * Note: The program assumes a specific format for the input data and FASTQ output.
 *       It is critical to ensure that the input data is correctly formatted and that
 *       the 'process' function is implemented to meet the requirements for the
 *       desired sequence processing or simulation.
 */
int main(int argc, char* argv[]) {
    
    // Validate arguments
    if (argc != 4) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [outfile1.fastq.gz] [outfile2.fastq.gz]" << std::endl;
        return -1;  
    }

    // validate input file
    GzFile infile( argv[1], "rb");
    if (!infile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // Open output file
    GzFile outfile1( argv[2], "wb" );
    if ( !outfile1 ) {
        std::cerr << "Error creating outfile " << argv[2] << std::endl;
        return -1;
    }

    // Open output file
    GzFile outfile2( argv[3], "wb" );
    if ( !outfile2 ) {
        std::cerr << "Error creating outfile " << argv[3] << std::endl;
        return -1;
    }

    char buffer[BUFFERSIZE];
    std::istringstream iss;
    std::string sequence, id;

    size_t read_count = 0;

    while (true) {
        
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        // get first sequence
        infile.gets(buffer, BUFFERSIZE);

        iss.clear();
        iss.str( buffer );

        iss >> sequence >> sequence >> sequence >> sequence >> sequence >> sequence >> sequence; 

        process( sequence );

        // get second sequence
        infile.gets(buffer, BUFFERSIZE);

        iss.clear();
        iss.str( buffer );

        iss >> id >> id;

        // print them to their file
        outfile1.printf("@%s\n", id.c_str());
        outfile1.printf("%s\n", sequence.c_str());

        iss >> sequence >> sequence >> sequence >> sequence >> sequence;

        process( sequence );

        outfile2.printf("@%s\n", id.c_str());
        outfile2.printf("%s\n", sequence.c_str());

        outfile1.printf(">%s\n", id.c_str());
        outfile1.printf("!!!\n");

        outfile2.printf(">%s\n", id.c_str());
        outfile2.printf("!!!\n");
        
        infile.gets(buffer, BUFFERSIZE);

        read_count++; 
        // if ( read_count == READ_COUNT ) {
        //     break;
        // }
    }

    std::cout << "Total number of processed reads: " << read_count << std::endl;

    return 0;
}
