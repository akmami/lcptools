#ifndef   ENCODING_CPP
#define   ENCODING_CPP

#include <map>

namespace lcp {

    static int coefficients[128];
    static char characters[128];
    static int dict_bit_size;

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

    int init_coefficients(bool verbose=false) {
        
        // init coefficients A/a=0, T/t=3, G/g=2, C/c=1
        for (int current_index = 0; current_index < 128; current_index++) {
            coefficients[current_index] = -1;
            characters[current_index] = 126;
        }
        coefficients['A'] = 0; coefficients['a'] = 0;
        coefficients['T'] = 3; coefficients['t'] = 3;
        coefficients['G'] = 2; coefficients['g'] = 2;
        coefficients['C'] = 1; coefficients['c'] = 1;
        
        characters[0] = 'A';
        characters[1] = 'C';
        characters[2] = 'G';
        characters[3] = 'T';

        dict_bit_size = 2;

        if (verbose) encoding_summary();

        return 0;
    };

    int init_coefficients(std::map<char, int> map, bool verbose=false) {

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
            
            coefficients[it->first] = it->second;
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
};

#endif