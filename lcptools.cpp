#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fstream>
#include "lps.h"


#define MAX_LINE_LENGTH     1024
#define SEQUENCE_CAPACITY   250000000


void print_usage( const char* lcptools ) {
    std::cout << "Usage: " << lcptools << " <command> <filename> <lcp-level> [sequence-size]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  falcpt   Process the fasta file." << std::endl;
    // std::cout << "  fqlcpt   Process the fasta file." << std::endl;
    std::cout << "File extensions:" << std::endl;
    std::cout << "  .fasta, .fa, .fastq, .fq" << std::endl;
};

bool validate_extension( std::string& infilename ) {
    std::string valid_extensions[] = { ".fasta", ".fa", ".fastq", ".fq" };

    for ( uint32_t i = 0; i < 4; i++ ) {
        if ( valid_extensions[i].size() < infilename.size() &&
            infilename.compare(infilename.size() - valid_extensions[i].size(), valid_extensions[i].size(), valid_extensions[i]) == 0 ) {
            return true;
        }
    }
    return false;
};

bool isNumber( const char* str ) {
    if (str == nullptr || *str == '\0') {
        return false;
    }

    char* end;
    errno = 0;
    long val = std::strtol(str, &end, 10);

    (void)val;

    if (errno != 0 || *end != '\0') {
        return false;
    }

    return true;
};


void done( std::ofstream &out ) {
    bool isDone = true;
    out.write(reinterpret_cast<const char*>(&isDone), sizeof(isDone));    
    out.close();
};

int process_fasta( const std::string& infilename, std::string& outfilename, const int lcp_level, const int sequence_size ) {
    
    std::fstream infile;
    infile.open(infilename, std::ios::in);
    
    std::ofstream outfile;
    outfile.open(outfilename);

    if ( ! infile.is_open() || ! outfile.is_open() ) {  
        std::cout << "Error opening file" << std::endl;
        return 1;
    }

    std::string sequence, line;
    sequence.reserve(sequence_size);
    
    // Initialize lcp encoding
    lcp::encoding::init();

    while ( getline( infile, line ) ) {

        if (line[0] != '>') {
            sequence += line;
            continue;
        }

        // process previous chromosome before moving into new one
        if ( 0 < sequence.size() ) {
            lcp::lps* str = new lcp::lps(sequence);
            str->deepen(lcp_level);
            str->write(outfile);
            
            delete str;

            sequence.clear();
        }
    }

    if ( sequence.size() != 0 ) {
        lcp::lps* str = new lcp::lps(sequence);
        str->deepen(lcp_level);
        str->write(outfile);

        delete str;
    }

    done( outfile );

    infile.close();
    outfile.close();

    return 0;
}

int main( int argc, char *argv[] ) {

    if ( argc < 4 ) {
        print_usage( argv[0] );
        return 1;
    }

    std::string command = std::string(argv[1]);
    std::string infilename = std::string(argv[2]);

    if ( command != "falcpt" ) {
        std::cout << "Error: Unsupported command " << command << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    if ( !validate_extension( infilename ) ) {
        std::cout << "Error: Invalid file extension. Supported extensions are .fasta, .fa, .fastq, .fq" << std::endl;
        return 1;
    }

    if ( !isNumber( argv[3] ) ) {
        std::cout << "Error: The lcp level argument must be a positive integer." << std::endl;
        return 1;
    }

    int lcp_level = atoi( argv[3] );
    int sequence_size = SEQUENCE_CAPACITY;

    if ( argc == 5 ) {
        if ( !isNumber( argv[4] ) ) {
            std::cout << "Error: The sequence size argument must be a positive integer." << std::endl;
            return 1;
        }
        sequence_size = atoi( argv[4] );
    }

    // generate output infilename
    std::string outfilename = infilename + ".lcpt";
    
    std::cout << "Output: " << outfilename << std::endl;

    process_fasta( infilename, outfilename, lcp_level, sequence_size );
    // todo: fastq

    return 0;
};