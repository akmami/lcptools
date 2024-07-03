#ifndef   ENCODING_H
#define   ENCODING_H

#include <map>
#include <fstream>
#include <iostream>

namespace lcp {
    
    extern int coefficients[128];
    extern int reverse_complement_coefficients[128];
    extern char characters[128];
    extern int dict_bit_size;

    void encoding_summary();
    int init_coefficients(bool verbose=false);
    int init_coefficients(std::map<char, int> map, std::map<char, int> rc_map, bool verbose=false);
    int init_coefficients(std::string encoding_file, bool verbose=false);
};

#endif
