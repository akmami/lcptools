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
    lcp::core core1(p1, 3, 0, 0, 10); // 101 in binary
    lcp::core core2(p2, 3, 1, 0, 10); // 111 in binary

    core1.compress(&core2);

    // expected result after compressing 101 and 111 is 10 (binary) => 2 in decimal
    assert(core1.p[0] == 0b10 && "Compressed core's label should be 0b10");
    assert(core1.size == 2 && "Compressed core's label length should be 2");
    assert(core1.label == 0 && "Core's label should be 1");
    
    log("...  test_core_compress passed!");
};


void test_core_constructors() {

    // test constructor with label, label_length, start, and end
    ublock* p1 = new ublock[1]; 
    p1[0] = 0b1111;
    lcp::core core1(p1, 4, 0, 0, 10); // 1111 in binary
    assert(core1.p[0] == 0b1111 && "Label should be 0b1111");
    assert(core1.size == 4 && "Label length should be 4");
    assert(core1.label == 0 && "Core label should be 1");
    assert(core1.start == 0 && "Start should be 0");
    assert(core1.end == 10 && "End should be 10");

    lcp::init_coefficients();

    // test constructor with core iterators
    std::string str2 = "CAGTT"; // binary 010010 that will be used to create core
    ublock* p = new ublock[1];
    *p = 0b1110; // binary 1110
    std::vector<lcp::core*> cores = { new lcp::core(str2.begin(), str2.begin()+3, 0, 3),
                                      new lcp::core(p, 4, 1, 3, 5) };
    lcp::core core4(cores.begin(), cores.end());
    assert(core4.p[0] == 0b0100101110 && "Concatenated label should be 0b0100101110");
    assert(core4.size == 10 && "Label length should be 10");
    // cleanup dynamically allocated cores
    for( std::vector<lcp::core*>::iterator it = cores.begin(); it != cores.end(); it++ ) {
        delete *it;
    }
    cores.clear();
    
    // test constructor with string iterators
    std::string str5 = "CAGAAATATGCGTAGTC";
    lcp::core core5(str5.begin(), str5.end(), 0, 17);
    assert(core5.p[0] == 0b01 && "First part of concatenated label should be 0b01");
    assert(core5.p[1] == 0b00100000001100111001101100101101 && "Second part of concatenated label should be 0b00100000001100111001101100101101");
    assert(core5.size == 34 && "Label length should be 34");
    
    log("...  test_core_constructors passed!");
};


void test_core_concatination() {


    std::string str = "CAGTTCAGGGGGGT";
    std::vector<lcp::core*> cores = { new lcp::core(str.begin(), str.begin()+3, 0, 3),      // 6 bits
                                      new lcp::core(str.begin()+2, str.begin()+6, 2, 6),    // 8 bits
                                      new lcp::core(str.begin()+5, str.begin()+8, 5, 8),    // 6 bits
                                      new lcp::core(str.begin()+6, str.begin()+14, 6, 14)};  // 16 bits
    
    lcp::core core(cores.begin(), cores.end());
    assert(core.p[0] == 0b0100 && "First part of concatenated label should be 0b0100");
    assert(core.p[1] == 0b10101111010100100010101010101011 && "First part of concatenated label should be 0b10101111010100100010101010101011");
    assert(core.size == 36 && "Label length should be 36");
   
    for( std::vector<lcp::core*>::iterator it = cores.begin(); it != cores.end(); it++ ) {
        delete *it;
    }
    cores.clear();

    log("...  test_core_concationation passed!");
}


void test_core_file_io() {

    // create a core object and write it to a file
    ublock* p1 = new ublock[1]; 
    p1[0] = 0b1011;
    lcp::core core1(p1, 4, 8, 0, 10); // 1111 in binary
    std::ofstream outfile("core_test.dat", std::ios::binary);
    core1.write(outfile);
    outfile.close();

    // read the core object from the file
    std::ifstream infile("core_test.dat", std::ios::binary);
    lcp::core core2(infile);
    infile.close();

    assert(core2.p[0] == 0b1011 && "Label should be 0b1011 after reading from file");
    assert(core2.size == 4 && "Label length should be 4 after reading from file");
    assert(core2.end == 10 && "End should be 10 after reading from file");
    assert(core2.label == 8 && "Core's label should be 8");

    // clean up the test file
    std::remove("core_test.dat");

    log("...  test_core_file_io passed!");
};


void test_core_operator_overloads() {

    ublock* p1 = new ublock[1];
    p1[0] = 0b1010;
    ublock* p2 = new ublock[1];
    p2[0] = 0b1010;
    ublock* p3 = new ublock[1];
    p3[0] = 0b101;
    lcp::core core1(p1, 4, 0, 0, 10);  // 1010 in binary
    lcp::core core2(p2, 4, 1, 0, 10);  // 1010 in binary
    lcp::core core3(p3, 3, 2, 0, 10);  // 101 in binary

    assert((core1 == core2) && "core1 should be equal to core2");
    assert((core1 != core3) && "core1 should not be equal to core3");
    assert((core3 < core1) && "core3 should be less than core1");
    assert((core1 > core3) && "core1 should be greater than core3");
    assert((core1 >= core2) && "core1 should be greater than or equal to core2");
    assert((core3 <= core1) && "core3 should be less than or equal to core1");

    log("...  test_core_operator_overloads passed!");
};


int main() {
    log("Running test_core...");

    test_core_compress();
    test_core_constructors();
    test_core_concatination();
    test_core_file_io();
    test_core_operator_overloads();

    log("All tests in test_core completed successfully!");

    return 0;
};
