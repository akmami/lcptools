#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include "GzFile.hpp"
#include <vector>
#include <algorithm>


struct MafRead {
    char id[IDSIZE];
    size_t genome_num;
    long pos;
    long length;
};

bool cmpr(const MafRead& a, const MafRead& b) {
    return a.genome_num < b.genome_num || ( a.genome_num == b.genome_num && a.pos < b.pos);
};

/**
 * A function to remove '-' from the given string
 */
void process( char str[BUFFERSIZE] ) {
    
    size_t writeIndex = 0, readIndex = 0;

    for ( ; readIndex < BUFFERSIZE && str[readIndex] != '\n'; readIndex++) {
        if (str[readIndex] != '-') {
            str[writeIndex++] = str[readIndex];
        }
    }

    str[writeIndex] = '\0';
};

int main(int argc, char **argv) {

    // Validate arguments
    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [outfile]" << std::endl;
        return -1;  
    }

    // validate input file
    GzFile maffile( argv[1], "rb");
    if (!maffile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // Open output file
    std::ofstream outfile(argv[2]);
    if ( !outfile.good() ) {
        std::cerr << "Error creating outfile " << argv[1] << std::endl;
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

        if ( maffile.gets(buffer, sizeof(buffer)) == Z_NULL) {
            // End of file or an error
            if ( ! maffile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        ++lineCount;

        if (lineCount % 4 == 2) {

            // remove '-' from sequence
            process(buffer);

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

            if ( sscanf(id, "%c%ld_%ld", &id_prefix, &genome_num, &sim_res_num) != 3) { // if sscanf successfully parsed 3 items
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

            // if ( genome_num == 1 ) {
            //     continue;
            // }

            // if ( lineCount < 16 ) {
            //     continue;
            // }

            // break;
        }
    }

    std::sort(reads.begin(), reads.end(), cmpr);

    for (std::vector<struct MafRead>::iterator it = reads.begin(); it != reads.end(); ++it) {
        std::cout << it->id << " " << it->genome_num << " " << it->pos << " " << it->length << std::endl;
    }

    // Write Header (optional)
    outfile << "H\tVN:Z:1.0\n";

    // Write Segments
    for (std::vector<struct MafRead>::iterator it = reads.begin(); it != reads.end(); ++it) {
        outfile << "S\t" << it->id << "\t.\n"; // put . instead of sequence string
    }

    long overlap, prev_pos, prev_length, cur_pos, cur_length;
    // Write Links
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

    return 0;
};
