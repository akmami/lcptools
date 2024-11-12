/**
 * @file encoding.cpp
 * @brief Implementation of encoding functions.
 *
 * This file contains the implementation of encoding functions used to initialize
 * the alphabet with their corresponding coefficients. The encodings support initialization 
 * with default coefficients, specific coefficients, or by reading coefficients from a file.
 */

#include "encoding.h"


namespace lcp {

    int alphabet[128];
    int rc_alphabet[128];
    char characters[128];
    int alphabet_bit_size;

    namespace encoding {
        
        void summary() {
            std::cout << "# Alphabet encoding summary" << std::endl;
            std::cout << "# Coefficients: ";
            for(int i=0; i<128; i++) {
                if (alphabet[i] != -1) {
                    std::cout << char(i) << ":" << alphabet[i] << " ";
                }
            }
            std::cout << std::endl;
            std::cout << "# Alphabet bit size: " << alphabet_bit_size << std::endl;
        };

        int init(bool verbose) {
            
            // init coefficients A/a=0, T/t=3, G/g=2, C/c=1
            for (int current_index = 0; current_index < 128; current_index++) {
                alphabet[current_index] = -1;
                characters[current_index] = 126;
            }
            alphabet['A'] = 0; alphabet['a'] = 0;
            alphabet['T'] = 3; alphabet['t'] = 3;
            alphabet['G'] = 2; alphabet['g'] = 2;
            alphabet['C'] = 1; alphabet['c'] = 1;

            rc_alphabet['A'] = 3; rc_alphabet['a'] = 3;
            rc_alphabet['T'] = 0; rc_alphabet['t'] = 0;
            rc_alphabet['G'] = 1; rc_alphabet['g'] = 1;
            rc_alphabet['C'] = 2; rc_alphabet['c'] = 2;
            
            characters[0] = 'A';
            characters[1] = 'C';
            characters[2] = 'G';
            characters[3] = 'T';

            alphabet_bit_size = 2;

            if (verbose) summary();

            return 0;
        };

        int init(std::map<char, int> map, std::map<char, int> rc_map, bool verbose) {

            // init coefficients A/a=0, T/t=3, G/g=2, C/c=1
            for (int current_index = 0; current_index < 128; current_index++) {
                alphabet[current_index] = -1;
                characters[current_index] = 126;
            }

            std::map<char, int>::iterator it = map.begin();
            int max = it->second;
            
            while (it != map.end()) {
                if (it->second < 0)
                    throw std::invalid_argument("Invalid value given.");
                
                alphabet[static_cast<unsigned char>(it->first)] = it->second;
                characters[it->second] = it->first;
                if (max < it->second) {
                    max = it->second;
                }
                it++;
            }

            // init reverse complement
            it = rc_map.begin();

            while (it != rc_map.end()) {
                if (it->second < 0)
                    throw std::invalid_argument("Invalid value given.");
                
                rc_alphabet[static_cast<unsigned char>(it->first)] = it->second;
                characters[it->second] = it->first;
                if (max < it->second) {
                    max = it->second;
                }
                it++;
            }
            
            int bit_count = 0;
            while(max > 0) {
                bit_count++;
                max = max / 2;
            }

            alphabet_bit_size = bit_count;

            if (verbose) summary();

            return 0;
        };
        
        int init(std::string encoding_file, bool verbose) {
            std::map<char, int> map, rev_map;
            std::ifstream encodings;
            encodings.open(encoding_file);

            char character;
            int encoding, rev_encoding;
            while( encodings >> character >> encoding >> rev_encoding ) {
                map[character] = encoding;
                rev_map[character] = rev_encoding;
            }
            
            return init(map, rev_map, verbose);
        };

    };
};