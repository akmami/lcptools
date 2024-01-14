#include <fstream>
#include <iostream>
#include <sstream>
#include "GzFile.hpp"


#define BUFFERSIZE  100000

/**
 * A function to remove '-' from the given string
 */
void process( char str[BUFFERSIZE], std::string &line ) {
    
    size_t writeIndex = 0, readIndex = 0;

    for ( ; readIndex < BUFFERSIZE && str[readIndex] != '\n'; readIndex++) {
        if (str[readIndex] != '-') {
            str[writeIndex++] = str[readIndex];
        }
    }

    str[writeIndex] = '\0';

    line.clear();
    line.append(str);
};

int main(int argc, char **argv) {

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

    // open output file
    std::ofstream outfile(argv[2]);
    if (!outfile) {
        std::cerr << "Error creating outfile " << argv[1] << std::endl;
        return 1;
    }

    // variables
    std::string line, seq1, seq2;
    char buffer[BUFFERSIZE];

	// read file
    while ( true ) {

        if ( maffile.gets(buffer, sizeof(buffer)) == Z_NULL) {
            // End of file or an error
            if ( ! maffile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }
        // line is 'a'

        // get raw sequence 1
        maffile.gets(buffer, sizeof(buffer));
        
        // remove '-' from raw sequence 1
        process(buffer, line);

        // parse string
        std::istringstream iss(line);

        // access the 7th column
        iss >> seq1 >> seq1 >> seq1 >> seq1 >> seq1 >> seq1 >> seq1; 

        outfile << seq1 << std::endl;

        line.clear();

        // get raw sequence 2
        maffile.gets(buffer, sizeof(buffer));

        // remove '-' from raw sequence 2
        process(buffer, line);
        
        // parse string
        iss.clear();
        iss.str(line);

        // access the 7th column
        iss >> seq2 >> seq2 >> seq2 >> seq2 >> seq2 >> seq2 >> seq2;

        outfile << seq2 << std::endl;

        // empty line
        maffile.gets(buffer, BUFFERSIZE);
    }

    return 0;
};
