#include "encoding.h"

namespace lcp {

    int coefficients[128];
    int reverse_complement_coefficients[128];
    char characters[128];
    int dict_bit_size;

    void encoding_summary() {
        std::cout << "# Alphabet encoding summary" << std::endl;
        std::cout << "# Coefficients: ";
        for(int i=0; i<128; i++) {
            if (coefficients[i] != -1) {
                std::cout << char(i) << ":" << coefficients[i] << " ";
            }
        }
        std::cout << std::endl;
        std::cout << "# Dictionary bit size: " << dict_bit_size << std::endl;
    };

    int init_coefficients(bool verbose) {
        
        // init coefficients A/a=0, T/t=3, G/g=2, C/c=1
        for (int current_index = 0; current_index < 128; current_index++) {
            coefficients[current_index] = -1;
            characters[current_index] = 126;
        }
        coefficients['A'] = 0; coefficients['a'] = 0;
        coefficients['T'] = 3; coefficients['t'] = 3;
        coefficients['G'] = 2; coefficients['g'] = 2;
        coefficients['C'] = 1; coefficients['c'] = 1;

        reverse_complement_coefficients['A'] = 3; reverse_complement_coefficients['a'] = 3;
        reverse_complement_coefficients['T'] = 0; reverse_complement_coefficients['t'] = 0;
        reverse_complement_coefficients['G'] = 1; reverse_complement_coefficients['g'] = 1;
        reverse_complement_coefficients['C'] = 2; reverse_complement_coefficients['c'] = 2;
        
        characters[0] = 'A';
        characters[1] = 'C';
        characters[2] = 'G';
        characters[3] = 'T';

        dict_bit_size = 2;

        if (verbose) encoding_summary();

        return 0;
    };

    int init_coefficients(std::map<char, int> map, std::map<char, int> rc_map, bool verbose) {

        // init coefficients A/a=0, T/t=3, G/g=2, C/c=1
        for (int current_index = 0; current_index < 128; current_index++) {
            coefficients[current_index] = -1;
            characters[current_index] = 126;
        }

        std::map<char, int>::iterator it = map.begin();
        int max = it->second;
        
        while (it != map.end()) {
            if (it->second < 0)
                throw std::invalid_argument("Invalid value given.");
            
            coefficients[static_cast<unsigned char>(it->first)] = it->second;
            characters[it->second] = it->first;
            if (max < it->second) {
                max = it->second;
            }
            it++;
        }

        // init reverse complement
        it = rc_map.begin();

        while (it != map.end()) {
            if (it->second < 0)
                throw std::invalid_argument("Invalid value given.");
            
            reverse_complement_coefficients[static_cast<unsigned char>(it->first)] = it->second;
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

        dict_bit_size = bit_count;

        if (verbose) encoding_summary();

        return 0;
    };
    
    int init_coefficients(std::string encoding_file, bool verbose) {
    	std::map<char, int> map, rev_map;
        std::ifstream encodings;
        encodings.open(encoding_file);

        char character;
        int encoding, rev_encoding;
        while( encodings >> character >> encoding >> rev_encoding ) {
            map[character] = encoding;
            rev_map[character] = rev_encoding;
        }
        
        return init_coefficients(map, rev_map, verbose);
    };
};