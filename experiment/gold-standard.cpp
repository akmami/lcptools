#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include "GzFile.hpp"

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

    char buffer[BUFFERSIZE];
    std::string seq;
    int lineCount = 0;

    char id[IDSIZE];
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

            // access sequence
            iss >> seq >> seq >> seq >> seq >> seq >> seq >> seq;
        }

        if (lineCount % 4 == 3) {

            std::istringstream iss(buffer);

            iss >> id >> id;

            if ( sscanf(id, "%c%ld_%ld", &id_prefix, &genome_num, &sim_res_num) != 3) { // if sscanf successfully parsed 3 items
                printf("Error parsing the string\n");
            }

            outfile << "@" << id << std::endl;
            outfile << seq << std::endl;
            outfile << "+" << id << std::endl;
            for( int i = 0; i < seq.length(); i++ ) {
                outfile << "!";
            }
            outfile << "\n";
        }
    }

    return 0;
};
