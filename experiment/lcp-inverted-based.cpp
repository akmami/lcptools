#include <iostream>
#include <sstream>
#include "../utils/GzFile.hpp"
#include "InvertedIndex.hpp"
#include "../utils/process.cpp"
#include "../string.cpp"


#define READCOUNT   1

int main(int argc, char **argv) {

    // validate arguments
    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [lcp-level]" << std::endl;
        return -1;
    }

    // validate input file
    GzFile infile(argv[1], "rb");
    if (!infile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    int lcp_level = atoi(argv[2]);

    // variables
    char buffer[BUFFERSIZE];
    std::istringstream iss;
    std::string id, str;
    bool isSequenceValid;

    std::map<std::string, doc_id_type> str2id;
    std::map<doc_id_type, std::string> id2str;
    doc_id_type id_index = 1;

    lcp::init_coefficients();
    InvertedIndex *inverted_index = new InvertedIndex();

    std::cout << "Constructing inverted index..." << std::endl;

    while (true) {
        
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        infile.gets(buffer, BUFFERSIZE);    // get first sequence

        infile.gets(buffer, BUFFERSIZE);    // get second sequence

             // if ( id_index == READCOUNT ) {
        //     break;
        // }   iss.clear();
        iss.str(buffer);
        iss >> id >> id;                        // get ID
        iss >> str >> str >> str >> str >> str; // get sequence

        isSequenceValid = process(str);

        if ( ! isSequenceValid ) {
            infile.gets(buffer, BUFFERSIZE);    // read empty line
            continue;
        }

        str2id[id] = id_index;                  // store ID and its corresponding integer value
        id2str[id_index] = id;

        // Process sequence
        lcp::string *str_error = new lcp::string(str);
        
        str_error->deepen(lcp_level);

        inverted_index->insert(id_index, str_error);

        id_index++;

        infile.gets(buffer, BUFFERSIZE);        // read empty line

        delete str_error;

        if ( id_index == READCOUNT ) {
            break;
        }
    }

    std::cout << "Inverted index structure constructed." << std::endl;

    infile.rewind();

    id_index = 1;
    size_t correctly_found = 0, total_found = 0;

    std::cout << "Retrieveing max indeces." << std::endl;

    while (true) {
        
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        infile.gets(buffer, BUFFERSIZE);            // get first sequence

        iss.clear();
        iss.str(buffer);
        iss >> str >> str >> str >> str >> str >> str >> str;  // extract sequence

        isSequenceValid = process(str);

        if ( ! isSequenceValid ) {
            infile.gets(buffer, BUFFERSIZE);
            infile.gets(buffer, BUFFERSIZE);
            continue;
        }

        // Process sequence
        lcp::string *str_original = new lcp::string(str);
        str_original->deepen(lcp_level);

        // Process reverse complement
        reverseComplement( str );
        lcp::string *str_original_rev = new lcp::string(str);
        str_original_rev->deepen(lcp_level);
        
        std::pair<doc_id_type, size_t> result = inverted_index->retrieveMaxDocIdAndCount(0, str_original);
        std::pair<doc_id_type, size_t> resultRev = inverted_index->retrieveMaxDocIdAndCount(0, str_original_rev);

        if ( result.second > resultRev.second && result.first == id_index ) {
            correctly_found++;
        } else if ( resultRev.second > result.second && resultRev.first == id_index ) {
            correctly_found++;
        }
        
        total_found++;
        id_index++;

        infile.gets(buffer, BUFFERSIZE);        // get second read
        infile.gets(buffer, BUFFERSIZE);        // read empty line

        delete str_original;

        if ( id_index == READCOUNT ) {
            break;
        }
    }

    std::cout << "Read retrieved." << std::endl;

    std::cout << "read count: " << id_index - 1 << std::endl; 

    std::cout << "Correctly found most similar one count: " << correctly_found << std::endl;
    std::cout << "Total number of reads being processed: " << total_found << std::endl;

    inverted_index->summary();

    delete inverted_index;

    return 0;
}
