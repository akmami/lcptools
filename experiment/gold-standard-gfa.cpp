/**
 * @file    gold-standard-gfa.cpp
 * @brief   MAF File Parsing and Processing Tool
 *
 * This program is designed to read, parse, and process data from a MAF 
 * (Multiple Alignment Format) file. It extracts relevant information from 
 * the MAF file, organizes it into a structured format, and then outputs 
 * the processed data.
 *
 * The program performs the following operations:
 * - Validates and opens the input and output files.
 * - Parses the MAF file to extract read information, including IDs, positions,
 *   and lengths.
 * - Sorts the reads based on specific compare function implemented in MafRead.hpp
 * - Outputs the processed reads, including segments and links, to the specified 
 *   output file in a defined format.
 *
 * Usage: <ExecutableName> <InputMafFile.maf.gz> <OutputFile.gfa>
 */


#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include "GzFile.hpp"
#include "MafRead.hpp"
#include <vector>
#include <algorithm>


bool mr_cmpr(const struct MafRead& a,const struct MafRead& b) {
    return a.genome_num < b.genome_num || ( a.genome_num == b.genome_num && a.pos < b.pos);
};


/**
 * @brief The entry point of the program for parsing and processing MAF files into GFA format.
 *
 * The main function coordinates the processing of a MAF file. It reads the file, extracts
 * read information, and sorts these reads based on genome numbers and positions. The program
 * supports two modes of operation: '--all' for exhaustive link generation between reads and 
 * '--simple' for generating links only between consecutive reads. The processed data is then 
 * outputted to a GFA formatted file.
 *
 * Usage: <ExecutableName> <InputMafFile.maf.gz> <OutputFile.gfa> [--all | --simple]
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return Returns 0 on successful execution, -1 on argument validation failure, or 1 on file operation errors.
 */
int main(int argc, char **argv) {

    // Validate arguments
    if (argc != 4) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [outfile] [--all | --simple]" << std::endl;
        return -1;  
    }

    // validate input file
    GzFile infile( argv[1], "rb");
    if (!infile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream outfile(argv[2]);
    if ( !outfile.good() ) {
        std::cerr << "Error creating outfile " << argv[2] << std::endl;
        return 1;
    }

    std::string operation(argv[3]);
    bool isAll = false;

    if ( operation == "--all" ) {
        isAll = true;
    } else if ( operation == "--simple" ) {
        isAll = false;
    } else {
        std::cerr << "Wrong input: " << argv[0] << " [infile] [outfile] [--all | --simple]" << std::endl;
        return -1; 
    }

    std::vector<struct MafRead> reads;

    char buffer[BUFFERSIZE];
    std::string seq;
    int lineCount = 0;

    char id[128];
    char id_prefix; // To store the prefix character which is S set by pbsim3
    size_t genome_num;
    long sim_res_num;
    long pos;

    while (true) {

        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL ) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        ++lineCount;

        if (lineCount % 4 == 2) {

            std::istringstream iss(buffer);

            // Ignore the first 2 columns and access 3rd
            iss >> seq >> seq >> seq;

            pos = atol( seq.c_str() );

            // Access the 7th column
            iss >> seq >> seq >> seq >> seq;
        }

        if (lineCount % 4 == 3) {

            std::istringstream iss(buffer);

            iss >> id >> id; 

            // if sscanf successfully parsed 3 items
            if ( sscanf(id, "%c%ld_%ld", &id_prefix, &genome_num, &sim_res_num) != 3) { 
                printf("Error parsing the string\n");
                continue;
            }

            struct MafRead read;
            strncpy(read.id, id, IDSIZE - 1);
            read.id[IDSIZE - 1] = '\0';

            read.genome_num = genome_num;
            read.pos = pos;
            read.length = seq.size();

            reads.push_back( read );
        }
    }

    std::sort(reads.begin(), reads.end(), mr_cmpr);

    // Write Header (optional)
    outfile << "H\tVN:Z:1.0\n";

    // Write Segments
    for (std::vector<struct MafRead>::iterator it = reads.begin(); it != reads.end(); ++it) {
        outfile << "S\t" << it->id << "\t.\n"; // put . instead of sequence string
    }

    long overlap, prev_pos, prev_length, cur_pos, cur_length;
    
    // Write Links
    if (!isAll) {
        for (std::vector<struct MafRead>::iterator it = reads.begin() + 1; it != reads.end(); ++it) {
            
            if ( (it - 1)->genome_num == (it)->genome_num ) {

                prev_pos = (it - 1)->pos;
                prev_length = (it - 1)->length;
                cur_pos = it->pos;
                cur_length = it->length;
                
                if ( ( overlap = std::max((long)0, ( std::min(cur_pos + cur_length, prev_pos + prev_length) - std::max(cur_pos, prev_pos) ) ) ) > 0 ) {
                    outfile << "L\t" << (it-1)->id << "\t+\t" << it->id << "\t-\t" << overlap << "M\n";
                }
            }
        }
    } else {
        for (std::vector<struct MafRead>::iterator it1 = reads.begin(); it1 != reads.end(); ++it1) {
            for (std::vector<struct MafRead>::iterator it2 = it1 + 1; it2 != reads.end(); ++it2) {

                // Only compare reads from the same genome
                if (it1->genome_num == it2->genome_num) {

                    prev_pos = it1->pos;
                    prev_length = it1->length;
                    cur_pos = it2->pos;
                    cur_length = it2->length;

                    // Calculate overlap
                    overlap = std::max((long)0, (std::min(cur_pos + cur_length, prev_pos + prev_length) - std::max(cur_pos, prev_pos)));

                    // If there is an overlap, write the link
                    if (overlap > 0) {
                        outfile << "L\t" << it1->id << "\t+\t" << it2->id << "\t-\t" << overlap << "M\n";
                    } else {
                        break;
                    }
                }
            }
        }
    }

    return 0;
};
