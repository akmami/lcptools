#include <cassert>
#include <iostream>
#include <vector>
#include <fstream>
#include "core.h"


void log(const std::string& message) {
    std::cout << message << std::endl;
};


void test_core_compress() {

    ublock* p1 = new ublock[1];  
    p1[0] = 0b101;
    ublock* p2 = new ublock[1];
    p2[0] = 0b111;
    lcp::core core1(p1, 1, 32-3, 0, 10); // 101 in binary
    lcp::core core2(p2, 1, 32-3, 0, 10); // 111 in binary

    core1.compress(&core2);

    // expected result after compressing 101 and 111 is 10 (binary) => 2 in decimal
    assert(core1.p[0] == 2 && "Compressed core's label should be 2");
    assert(core1.block_number == 1 && "Compressed core's block number should be 1");
    assert(core1.start_index == 30 && "Compressed core's start index should be 30");
    
    log("test_core_compress passed!");
};


void test_core_constructors() {
    std::cout << "starting" << std::endl;
    // test constructor with label, label_length, start, and end
    ublock* p1 = new ublock[1]; 
    p1[0] = 0b1111;
    lcp::core core1(p1, 1, 32-4, 0, 10); // 1111 in binary
    assert(core1.p[0] == 15 && "Label should be 15");
    assert(core1.start_index == 28 && "Label length should be 4");
    assert(core1.start == 0 && "Start should be 0");
    assert(core1.end == 10 && "End should be 10");

    lcp::init_coefficients();

    // test constructor with core iterators
    std::string str2 = "CAGTT"; // binary 010010 that will be used to create core
    ublock* p = new ublock[1];
    *p = 0b1110; // binary 1110

    std::vector<lcp::core*> cores = { new lcp::core(str2.begin(), str2.end(), 0, 3),
                                                new lcp::core(p, 1, 28, 3, 5)};
    lcp::core core4(cores.begin(), cores.end());
    assert(core4.p[0] == 0b0100101110 && "Concatenated label should be 302");
    assert(core4.start_index == 22 && "Label length should be 22");

    // cleanup dynamically allocated cores
    for( std::vector<lcp::core*>::iterator it = cores.begin(); it != cores.end(); it++ ) {
        delete *it;
    }
    cores.clear();
    
    // test constructor with string iterators
    std::string str5 = "CAGAAATATGCGTAGTC";
    lcp::core core5(str5.begin(), str5.end(), 0, 17);
    assert(core5.p[0] == 0b01 && "First part of concatenated label should be 1");
    assert(core5.p[1] == 0b00100000001100111001101100101101 && "First part of concatenated label should be 1");
    assert(core5.start_index == 30 && "Label length should be 30");
    
    log("test_core_constructors passed!");
};


void test_core_file_io() {

    // create a core object and write it to a file
    ublock* p1 = new ublock[1]; 
    p1[0] = 0b1011;
    lcp::core core1(p1, 1, 32-4, 0, 10); // 1111 in binary
    std::ofstream outfile("core_test.dat", std::ios::binary);
    core1.write(outfile);
    outfile.close();

    // read the core object from the file
    std::ifstream infile("core_test.dat", std::ios::binary);
    lcp::core core2(infile);
    infile.close();

    assert(core2.p[0] == 11 && "Label should be 10 after reading from file");
    assert(core2.start_index == 28 && "Label length should be 28 after reading from file");
    assert(core2.block_number == 1 && "Block number should be 1 after reading from file");
    assert(core2.end == 10 && "End should be 10 after reading from file");

    // clean up the test file
    std::remove("core_test.dat");

    log("test_core_file_io passed!");
};


void test_core_operator_overloads() {

    ublock* p1 = new ublock[1];
    p1[0] = 0b1010;
    ublock* p2 = new ublock[1];
    p2[0] = 0b1010;
    ublock* p3 = new ublock[1];
    p3[0] = 0b101;
    lcp::core core1(p1, 1, 32-4, 0, 10);  // 1010 in binary
    lcp::core core2(p2, 1, 32-4, 0, 10);  // 1010 in binary
    lcp::core core3(p3, 1, 32-3, 0, 10);  // 101 in binary

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
