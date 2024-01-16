#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include "GzFile.hpp"

int main(int argc, char **argv) {

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
    if (!outfile) {
        std::cerr << "Error creating outfile " << argv[2] << std::endl;
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
        
        if ( infile.gets(buffer, sizeof(buffer)) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        ++lineCount;

        if (lineCount % 4 == 2) {

            std::istringstream iss(buffer);

            // access sequence
            iss >> seq >> seq >> seq >> seq >> seq >> seq >> seq;
        }

        if (lineCount % 4 == 3) {

            std::istringstream iss(buffer);

            iss >> id >> id;

            // if sscanf successfully parsed 3 items
            if ( sscanf(id, "%c%ld_%ld", &id_prefix, &genome_num, &sim_res_num) != 3) { 
                printf("Error parsing the string\n");
            }

            outfile.printf("@%s\n", id);
            outfile.printf("%s\n", seq);
            outfile.printf("+%s\n", id);
            for( int i = 0; i < seq.length(); i++ ) {
                outfile.printf("+%c", '!');
            }
            outfile.printf("\n");
        }
    }

    return 0;
};
