#include <cassert>
#include <iostream>
#include <fstream>
#include <vector>
#include "lps.h"
#include "core.h"


void log(const std::string& message) {
    std::cout << message << std::endl;
};


void test_lps_constructor() {

    lcp::init_coefficients();

    std::string test_string = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
    // create an lps object
    lcp::lps lps_obj(test_string, false);

    // cores to compare
    std::vector<lcp::core*> cores = {
        new lcp::core(new ublock[1]{0b100001}, 6, 0, 0, 0),  // Core: 100001 (6 bits)
        new lcp::core(new ublock[1]{0b00010111}, 8, 1, 0, 0),  // Core: 00010111 (8 bits)
        new lcp::core(new ublock[1]{0b011110}, 6, 2, 0, 0),  // Core: 011110 (6 bits)
        new lcp::core(new ublock[1]{0b11101011}, 8, 3, 0, 0),  // Core: 11101011 (8 bits)
        new lcp::core(new ublock[1]{0b101110}, 6, 4, 0, 0),  // Core: 101110 (6 bits)
        new lcp::core(new ublock[1]{0b100001}, 6, 5, 0, 0),  // Core: 100001 (6 bits)
        new lcp::core(new ublock[1]{0b000101010100}, 12, 6, 0, 0),  // Core: 000101010100 (12 bits)
        new lcp::core(new ublock[1]{0b010010}, 6, 7, 0, 0),  // Core: 010010 (6 bits)
        new lcp::core(new ublock[1]{0b1001010100}, 10, 8, 0, 0),  // Core: 1001010100 (10 bits)
        new lcp::core(new ublock[1]{0b010001}, 6, 9, 0, 0),  // Core: 010001 (6 bits)
        new lcp::core(new ublock[1]{0b100001}, 6, 10, 0, 0),  // Core: 100001 (6 bits)
        new lcp::core(new ublock[1]{0b010010}, 6, 11, 0, 0),  // Core: 010010 (6 bits)
        new lcp::core(new ublock[1]{0b10010100}, 8, 12, 0, 0),  // Core: 10010100 (8 bits)
        new lcp::core(new ublock[1]{0b01000010}, 8, 13, 0, 0),  // Core: 01000010 (8 bits)
        new lcp::core(new ublock[1]{0b100110}, 6, 14, 0, 0),  // Core: 100110 (6 bits)
        new lcp::core(new ublock[1]{0b10010100}, 8, 15, 0, 0),  // Core: 10010100 (8 bits)
        new lcp::core(new ublock[1]{0b010010}, 6, 16, 0, 0),  // Core: 010010 (6 bits)
        new lcp::core(new ublock[1]{0b100111}, 6, 17, 0, 0),  // Core: 100111 (6 bits)
        new lcp::core(new ublock[1]{0b100010}, 6, 18, 0, 0),  // Core: 100010 (6 bits)
        new lcp::core(new ublock[1]{0b100111}, 6, 19, 0, 0),  // Core: 100111 (6 bits)
        new lcp::core(new ublock[1]{0b010010}, 6, 20, 0, 0),  // Core: 010010 (6 bits)
        new lcp::core(new ublock[1]{0b00101011}, 8, 21, 0, 0),  // Core: 00101011 (8 bits)
        new lcp::core(new ublock[1]{0b111011}, 6, 22, 0, 0),  // Core: 111011 (6 bits)
        new lcp::core(new ublock[1]{0b100010}, 6, 23, 0, 0),  // Core: 100010 (6 bits)
        new lcp::core(new ublock[1]{0b00101000}, 8, 24, 0, 0),  // Core: 00101000 (8 bits)
        new lcp::core(new ublock[1]{0b100010}, 6, 25, 0, 0),  // Core: 100010 (6 bits)
        new lcp::core(new ublock[1]{0b100011}, 6, 26, 0, 0),  // Core: 100011 (6 bits)
        new lcp::core(new ublock[1]{0b010001}, 6, 27, 0, 0),  // Core: 010001 (6 bits)
        new lcp::core(new ublock[1]{0b010010}, 6, 28, 0, 0),  // Core: 010010 (6 bits)
        new lcp::core(new ublock[1]{0b101101}, 6, 29, 0, 0),  // Core: 101101 (6 bits)
        new lcp::core(new ublock[1]{0b11010111}, 8, 30, 0, 0)   // Core: 11010111 (8 bits)
    };
    
    // compare the resulting cores at level 1
    assert(lps_obj.cores->size() == cores.size() && "Core size at level 1 should match");
    for (size_t i = 0; i < lps_obj.cores->size(); ++i) {
        assert(*((*(lps_obj.cores))[i]) == *(cores[i]) && "Cores at level 1 should match");
    }

    // cleanup dynamically allocated cores
    for(std::vector<lcp::core*>::iterator it = cores.begin(); it != cores.end(); it++ ) {
        delete *it;
    }

    log("...  test_lps_constructor passed!");
};


void test_lps_file_io() {

    lcp::init_coefficients();

    std::string test_string = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
    lcp::lps lps_obj(test_string, false);

    // write to file
    std::string filename = "lps_test.dat";
    lps_obj.write(filename);

    // read from file
    std::ifstream infile(filename, std::ios::binary);
    lcp::lps lps_obj_from_file(infile);
    infile.close();

    // compare the read object with the original
    assert(lps_obj.level == lps_obj_from_file.level && "Level should match after reading from file");
    assert(lps_obj.cores->size() == lps_obj_from_file.cores->size() && "Core size should match after reading from file");
    for (size_t i = 0; i < lps_obj.cores->size(); ++i) {
        assert(*((*(lps_obj.cores))[i]) == *((*(lps_obj_from_file.cores))[i]) && "Cores should match after reading from file");
    }

    // clean up the test file
    std::remove(filename.c_str());

    log("...  test_lps_file_io passed!");
};


void test_lps_deepen() {

    lcp::init_coefficients();

    std::string test_string = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
    lcp::lps lps_obj(test_string, false);

    // peepen to level 2
    bool success = lps_obj.deepen(2);
    assert(success && "Deepening to level 2 should be successful");

    // peepen to level 3
    success = lps_obj.deepen(3);
    assert(success && "Deepening to level 3 should be successful");

    // attempt to deepen to a lower level (should not do anything)
    success = lps_obj.deepen(2);
    assert(!success && "Deepening to a lower level should be unsuccessful");

    log("...  test_lps_deepen passed!");
}

int main() {

    log("Running test_lps...");

    test_lps_constructor();
    test_lps_file_io();
    test_lps_deepen();

    log("All tests in test_lps completed successfully!");

    return 0;
}
