#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include "core.h"
#include "base_core.h"


void log(const std::string& message) {
    std::cout << message << std::endl;
};


void test_core_compress() {

    lcp::core core1(5, 3, 0, 10); // 101 in binary
    lcp::core core2(7, 3, 0, 10); // 111 in binary

    core1.compress(&core2);

    // expected result after compressing 101 and 111 is 10 (binary) => 2 in decimal
    assert(core1.label == 2 && "Compressed label should be 2");
    assert(core1.label_length == 2 && "Compressed label length should be 2");

    log("test_core_compress passed!");
};


void test_core_constructors() {

    // test constructor with label, label_length, start, and end
    lcp::core core1(15, 4, 0, 10); // 1111 in binary
    assert(core1.label == 15 && "Label should be 15");
    assert(core1.label_length == 4 && "Label length should be 4");
    assert(core1.start == 0 && "Start should be 0");
    assert(core1.end == 10 && "End should be 10");

    // test constructor with base_core iterators
    uchar* p1 = new uchar[1]; 
    p1[0] = {7}; // binary 111
    uchar* p2 = new uchar[1]; 
    p2[0] = {3}; // binary 11
    std::vector<lcp::base_core*> base_cores = {new lcp::base_core(p1, 1, 29, 0, 0),
                                               new lcp::base_core(p2, 1, 30, 0, 0)};
    lcp::core core2(base_cores.begin(), base_cores.end());
    assert(core2.label == 31 && "Concatenated label should be 31"); // (01 << 2) | 11 = 0111
    assert(core2.label_length == 5 && "Label length should be 5");
    p1 = NULL;
    p2 = NULL;

    // test constructor with core iterators
    std::vector<lcp::core*> cores = {new lcp::core(3, 2, 0, 5),
                                     new lcp::core(2, 2, 5, 10)};
    lcp::core core3(cores.begin(), cores.end());
    assert(core3.label == 14 && "Concatenated label should be 14"); // (11 << 2) | 10 = 1110
    assert(core3.label_length == 4 && "Label length should be 4");

    // cleanup dynamically allocated base_cores
    for(std::vector<lcp::base_core*>::iterator it = base_cores.begin(); it != base_cores.end(); it++ ) {
        delete *it;
    }

    // cleanup dynamically allocated cores
    for(std::vector<lcp::core*>::iterator it = cores.begin(); it != cores.end(); it++) {
        delete *it;
    }

    log("test_core_constructors passed!");
};


void test_core_file_io() {

    // create a core object and write it to a file
    lcp::core core1(10, 4, 0, 10); // 1010 in binary
    std::ofstream outfile("core_test.dat", std::ios::binary);
    core1.write(outfile);
    outfile.close();

    // read the core object from the file
    std::ifstream infile("core_test.dat", std::ios::binary);
    lcp::core core2(infile);
    infile.close();

    assert(core2.label == 10 && "Label should be 10 after reading from file");
    assert(core2.label_length == 4 && "Label length should be 4 after reading from file");
    assert(core2.start == 0 && "Start should be 0 after reading from file");
    assert(core2.end == 10 && "End should be 10 after reading from file");

    // clean up the test file
    std::remove("core_test.dat");

    log("test_core_file_io passed!");
};


void test_core_operator_overloads() {

    lcp::core core1(10, 4, 0, 10); // 1010 in binary
    lcp::core core2(10, 4, 0, 10); // 1010 in binary
    lcp::core core3(5, 3, 0, 10);  // 101 in binary

    assert((core1 == core2) && "core1 should be equal to core2");
    assert((core1 != core3) && "core1 should not be equal to core3");
    assert((core3 < core1) && "core3 should be less than core1");
    assert((core1 > core3) && "core1 should be greater than core3");
    assert((core1 >= core2) && "core1 should be greater than or equal to core2");
    assert((core3 <= core1) && "core3 should be less than or equal to core1");

    log("test_core_operator_overloads passed!");
};


int main() {
    log("Running test_core...");

    test_core_compress();
    test_core_constructors();
    test_core_file_io();
    test_core_operator_overloads();

    log("All tests in test_core completed successfully!");

    return 0;
};
