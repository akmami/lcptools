#include <iostream>
#include <fstream>
#include "GzFile.hpp"


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

        std::string id(buffer + 2);
        size_t firstUnderscore = id.find('_');

        if ( chrom == std::stoul( id.substr(0, firstUnderscore) ) ) {
            outfile.printf("%s", buffer);           // Print @id

            infile.gets(buffer, BUFFERSIZE);
            outfile.printf("%s", buffer);           // Write the sequence
            
            infile.gets(buffer, BUFFERSIZE);
            outfile.printf("%s", buffer);           // Write the '+' line

            infile.gets(buffer, BUFFERSIZE);
            outfile.printf("%s", buffer);           // Write the quality line

            continue;
        }

        infile.gets(buffer, BUFFERSIZE);
        infile.gets(buffer, BUFFERSIZE);
        infile.gets(buffer, BUFFERSIZE);
    }

    return 0;
}