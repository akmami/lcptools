#include <cassert>
#include <iostream>
#include <fstream>
#include <map>
#include "encoding.h"


void log(const std::string& message) {
    std::cout << message << std::endl;
};


void test_encoding_initialization_default() {

    lcp::init_coefficients(false);

    // check default coefficients
    assert(lcp::coefficients['A'] == 0 && "A should be encoded as 0");
    assert(lcp::coefficients['C'] == 1 && "C should be encoded as 1");
    assert(lcp::coefficients['G'] == 2 && "G should be encoded as 2");
    assert(lcp::coefficients['T'] == 3 && "T should be encoded as 3");
    assert(lcp::coefficients['a'] == 0 && "a should be encoded as 0");
    assert(lcp::coefficients['c'] == 1 && "c should be encoded as 1");
    assert(lcp::coefficients['g'] == 2 && "g should be encoded as 2");
    assert(lcp::coefficients['t'] == 3 && "t should be encoded as 3");

    // check reverse complement coefficients
    assert(lcp::reverse_complement_coefficients['A'] == 3 && "Reverse complement of A should be 3");
    assert(lcp::reverse_complement_coefficients['C'] == 2 && "Reverse complement of C should be 2");
    assert(lcp::reverse_complement_coefficients['G'] == 1 && "Reverse complement of G should be 1");
    assert(lcp::reverse_complement_coefficients['T'] == 0 && "Reverse complement of T should be 0");
    assert(lcp::reverse_complement_coefficients['a'] == 3 && "Reverse complement of a should be 3");
    assert(lcp::reverse_complement_coefficients['c'] == 2 && "Reverse complement of c should be 2");
    assert(lcp::reverse_complement_coefficients['g'] == 1 && "Reverse complement of g should be 1");
    assert(lcp::reverse_complement_coefficients['t'] == 0 && "Reverse complement of t should be 0");

    // check dictionary bit size
    assert(lcp::dict_bit_size == 2 && "Dictionary bit size should be 2");

    log("test_encoding_initialization_default passed!");
};


void test_encoding_initialization_from_map() {

    std::map<char, int> map = {{'A', 2}, {'C', 3}, {'G', 0}, {'T', 4}};
    std::map<char, int> rc_map = {{'A', 4}, {'C', 0}, {'G', 3}, {'T', 2}};

    lcp::init_coefficients(map, rc_map, false);

    // check coefficients
    assert(lcp::coefficients['A'] == 2 && "A should be encoded as 2");
    assert(lcp::coefficients['C'] == 3 && "C should be encoded as 3");
    assert(lcp::coefficients['G'] == 0 && "G should be encoded as 0");
    assert(lcp::coefficients['T'] == 4 && "T should be encoded as 4");

    // check reverse complement coefficients
    assert(lcp::reverse_complement_coefficients['A'] == 4 && "Reverse complement of A should be 4");
    assert(lcp::reverse_complement_coefficients['C'] == 0 && "Reverse complement of C should be 0");
    assert(lcp::reverse_complement_coefficients['G'] == 3 && "Reverse complement of G should be 3");
    assert(lcp::reverse_complement_coefficients['T'] == 2 && "Reverse complement of T should be 2");

    // check dictionary bit size
    assert(lcp::dict_bit_size == 3 && "Dictionary bit size should be 3");

    log("test_encoding_initialization_from_map passed!");
};


void test_encoding_initialization_from_file() {

    // create a temporary encoding file
    std::ofstream encoding_file("encoding_test.txt");
    encoding_file << "A 5 2\n";
    encoding_file << "C 3 3\n";
    encoding_file << "G 7 0\n";
    encoding_file << "T 8 1\n";
    encoding_file.close();

    lcp::init_coefficients("encoding_test.txt", false);

    // check coefficients
    assert(lcp::coefficients['A'] == 5 && "A should be encoded as 5");
    assert(lcp::coefficients['C'] == 3 && "C should be encoded as 3");
    assert(lcp::coefficients['G'] == 7 && "G should be encoded as 7");
    assert(lcp::coefficients['T'] == 8 && "T should be encoded as 8");

    // check reverse complement coefficients
    assert(lcp::reverse_complement_coefficients['A'] == 2 && "Reverse complement of A should be 2");
    assert(lcp::reverse_complement_coefficients['C'] == 3 && "Reverse complement of C should be 3");
    assert(lcp::reverse_complement_coefficients['G'] == 0 && "Reverse complement of G should be 0");
    assert(lcp::reverse_complement_coefficients['T'] == 1 && "Reverse complement of T should be 1");

    // check dictionary bit size
    assert(lcp::dict_bit_size == 4 && "Dictionary bit size should be 4");

    // clean up the temporary file
    std::remove("encoding_test.txt");

    log("test_encoding_initialization_from_file passed!");
};


int main() {
    log("Running test_encoding...");

    test_encoding_initialization_default();
    test_encoding_initialization_from_map();
    test_encoding_initialization_from_file();

    log("All tests in test_encoding completed successfully!");

    return 0;
};
