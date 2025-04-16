#include "core.h"
#include "lps.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <vector>

void log(const std::string &message) {
	std::cout << message << std::endl;
};

void test_lps_char_constructor() {

	lcp::encoding::init();

	const char *test_string1 = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
	// create an lps object
	lcp::lps lps_obj1(test_string1, strlen(test_string1));

    std::string test_string2 = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
    lcp::lps lps_obj2(test_string2);
	
    // compare the resulting cores at level 1
	assert(lps_obj1.size() == lps_obj2.size() && "Core size at level 1 should match");
    assert(lps_obj1 == lps_obj2 && "Cores at level 1 should match");

    lps_obj1.deepen();
    lps_obj2.deepen();

    // compare the resulting cores at level 1
	assert(lps_obj1.size() == lps_obj2.size() && "Core size at level 2 should match");
    assert(lps_obj1 == lps_obj2 && "Cores at level 2 should match");

	log("...  test_lps_char_constructor passed!");
};

void test_lps_char_rev_compl_constructor() {

	lcp::encoding::init();

	const char *test_string1 = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
	// create an lps object
	lcp::lps lps_obj1(test_string1, strlen(test_string1), false, true);

    std::string test_string2 = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
    lcp::lps lps_obj2(test_string2, false, true);
	
    // compare the resulting cores at level 1
	assert(lps_obj1.size() == lps_obj2.size() && "Core size at level 1 should match");
    assert(lps_obj1 == lps_obj2 && "Cores at level 1 should match");

    lps_obj1.deepen();
    lps_obj2.deepen();

    // compare the resulting cores at level 1
	assert(lps_obj1.size() == lps_obj2.size() && "Core size at level 2 should match");
    assert(lps_obj1 == lps_obj2 && "Cores at level 2 should match");

	log("...  test_lps_char_rev_compl_constructor passed!");
};


void test_lps_constructor() {

	lcp::encoding::init();

	std::string test_string = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
	// create an lps object
	lcp::lps lps_obj(test_string);

	// cores to compare
	std::vector<struct lcp::core *> cores;
	cores.resize(31);

	ublock *p0 = new ublock[1];
	p0[0] = 0b100001;
	cores[0] = new lcp::core(6, p0, 1, 0, 0);
	ublock *p1 = new ublock[1];
	p1[0] = 0b00010111;
	cores[1] = new lcp::core(8, p1, 2, 0, 0);
	ublock *p2 = new ublock[1];
	p2[0] = 0b011110;
	cores[2] = new lcp::core(6, p2, 3, 0, 0);
	ublock *p3 = new ublock[1];
	p3[0] = 0b11101011;
	cores[3] = new lcp::core(8, p3, 4, 0, 0);
	ublock *p4 = new ublock[1];
	p4[0] = 0b101110;
	cores[4] = new lcp::core(6, p4, 5, 0, 0);
	ublock *p5 = new ublock[1];
	p5[0] = 0b100001;
	cores[5] = new lcp::core(6, p5, 6, 0, 0);
	ublock *p6 = new ublock[1];
	p6[0] = 0b000101010100;
	cores[6] = new lcp::core(12, p6, 7, 0, 0);
	ublock *p7 = new ublock[1];
	p7[0] = 0b010010;
	cores[7] = new lcp::core(6, p7, 8, 0, 0);
	ublock *p8 = new ublock[1];
	p8[0] = 0b1001010100;
	cores[8] = new lcp::core(10, p8, 9, 0, 0);
	ublock *p9 = new ublock[1];
	p9[0] = 0b010001;
	cores[9] = new lcp::core(6, p9, 10, 0, 0);
	ublock *p10 = new ublock[1];
	p10[0] = 0b100001;
	cores[10] = new lcp::core(6, p10, 11, 0, 0);

	ublock *p11 = new ublock[1];
	p11[0] = 0b010010;
	cores[11] = new lcp::core(6, p11, 12, 0, 0);
	ublock *p12 = new ublock[1];
	p12[0] = 0b10010100;
	cores[12] = new lcp::core(8, p12, 13, 0, 0);
	ublock *p13 = new ublock[1];
	p13[0] = 0b01000010;
	cores[13] = new lcp::core(8, p13, 14, 0, 0);
	ublock *p14 = new ublock[1];
	p14[0] = 0b100110;
	cores[14] = new lcp::core(6, p14, 15, 0, 0);
	ublock *p15 = new ublock[1];
	p15[0] = 0b10010100;
	cores[15] = new lcp::core(8, p15, 16, 0, 0);
	ublock *p16 = new ublock[1];
	p16[0] = 0b010010;
	cores[16] = new lcp::core(6, p16, 17, 0, 0);
	ublock *p17 = new ublock[1];
	p17[0] = 0b100111;
	cores[17] = new lcp::core(6, p17, 18, 0, 0);
	ublock *p18 = new ublock[1];
	p18[0] = 0b100010;
	cores[18] = new lcp::core(6, p18, 19, 0, 0);
	ublock *p19 = new ublock[1];
	p19[0] = 0b100111;
	cores[19] = new lcp::core(6, p19, 20, 0, 0);
	ublock *p20 = new ublock[1];
	p20[0] = 0b010010;
	cores[20] = new lcp::core(6, p20, 21, 0, 0);

	ublock *p21 = new ublock[1];
	p21[0] = 0b00101011;
	cores[21] = new lcp::core(8, p21, 22, 0, 0);
	ublock *p22 = new ublock[1];
	p22[0] = 0b111011;
	cores[22] = new lcp::core(6, p22, 23, 0, 0);
	ublock *p23 = new ublock[1];
	p23[0] = 0b100010;
	cores[23] = new lcp::core(6, p23, 24, 0, 0);
	ublock *p24 = new ublock[1];
	p24[0] = 0b00101000;
	cores[24] = new lcp::core(8, p24, 25, 0, 0);
	ublock *p25 = new ublock[1];
	p25[0] = 0b100010;
	cores[25] = new lcp::core(6, p25, 26, 0, 0);
	ublock *p26 = new ublock[1];
	p26[0] = 0b100011;
	cores[26] = new lcp::core(6, p26, 27, 0, 0);
	ublock *p27 = new ublock[1];
	p27[0] = 0b010001;
	cores[27] = new lcp::core(6, p27, 28, 0, 0);
	ublock *p28 = new ublock[1];
	p28[0] = 0b010010;
	cores[28] = new lcp::core(6, p28, 29, 0, 0);
	ublock *p29 = new ublock[1];
	p29[0] = 0b101101;
	cores[29] = new lcp::core(6, p29, 30, 0, 0);
	ublock *p30 = new ublock[1];
	p30[0] = 0b11010111;
	cores[30] = new lcp::core(8, p30, 31, 0, 0);

	// compare the resulting cores at level 1
	assert(lps_obj.cores->size() == cores.size() && "Core size at level 1 should match");
	for (size_t i = 0; i < lps_obj.cores->size(); i++) {
		assert((*(lps_obj.cores))[i] == *(cores[i]) && "Cores at level 1 should match");
	}

	for (size_t i = 0; i < cores.size(); i++) {
		delete cores[i];
	}

	log("...  test_lps_constructor passed!");
};

void test_lps_file_io() {

	lcp::encoding::init();

	std::string test_string = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
	lcp::lps lps_obj(test_string);

	// write to file
	std::string filename = "lps_test.dat";
	std::ofstream outfile(filename);
	lps_obj.write(outfile);
	outfile.close();

	// read from file
	std::ifstream infile(filename, std::ios::binary);
	lcp::lps lps_obj_from_file(infile);
	infile.close();

	// compare the read object with the original
	assert(lps_obj.level == lps_obj_from_file.level && "Level should match after reading from file");
	assert(lps_obj.cores->size() == lps_obj_from_file.cores->size() && "Core size should match after reading from file");
	for (size_t i = 0; i < lps_obj.cores->size(); ++i) {
		assert((*(lps_obj.cores))[i] == (*(lps_obj_from_file.cores))[i] && "Cores should match after reading from file");
	}

	// clean up the test file
	std::remove(filename.c_str());

	log("...  test_lps_file_io passed!");
};

void test_lps_deepen() {

	lcp::encoding::init();

	std::string test_string = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";
	lcp::lps lps_obj(test_string, false);

	// deepen to level 2
	bool success = lps_obj.deepen(2);
	assert(success && "Deepening to level 2 should be successful");

	std::vector<struct lcp::core *> level2_cores;
	level2_cores.resize(11);

	ublock *p0 = new ublock[1];
	p0[0] = 0b00010001;
	level2_cores[0] = new lcp::core(8, p0, 1, 0, 0);
	ublock *p1 = new ublock[1];
	p1[0] = 0b00010011;
	level2_cores[1] = new lcp::core(8, p1, 2, 0, 0);
	ublock *p2 = new ublock[1];
	p2[0] = 0b1110011000;
	level2_cores[2] = new lcp::core(10, p2, 3, 0, 0);
	ublock *p3 = new ublock[1];
	p3[0] = 0b0110000010;
	level2_cores[3] = new lcp::core(10, p3, 4, 0, 0);
	ublock *p4 = new ublock[1];
	p4[0] = 0b001011101;
	level2_cores[4] = new lcp::core(9, p4, 4, 0, 0);
	ublock *p5 = new ublock[1];
	p5[0] = 0b111011011;
	level2_cores[5] = new lcp::core(9, p5, 5, 0, 0);
	ublock *p6 = new ublock[1];
	p6[0] = 0b11010001;
	level2_cores[6] = new lcp::core(8, p6, 6, 0, 0);
	ublock *p7 = new ublock[1];
	p7[0] = 0b00010001;
	level2_cores[7] = new lcp::core(8, p7, 7, 0, 0);
	ublock *p8 = new ublock[1];
	p8[0] = 0b0110010010;
	level2_cores[8] = new lcp::core(10, p8, 8, 0, 0);
	ublock *p9 = new ublock[1];
	p9[0] = 0b10110110;
	level2_cores[9] = new lcp::core(8, p9, 9, 0, 0);
	ublock *p10 = new ublock[1];
	p10[0] = 0b01100001;
	level2_cores[10] = new lcp::core(8, p10, 10, 0, 0);

	assert(lps_obj.cores->size() == level2_cores.size() && "Core size at level 2 should match");
	for (size_t i = 0; i < lps_obj.cores->size(); i++) {
		assert((*(lps_obj.cores))[i] == *(level2_cores[i]) && "Cores at level 2 should match");
	}

	// cleanup level 2 cores
	for (size_t i = 0; i < level2_cores.size(); i++) {
		delete level2_cores[i];
	}

	// test level 3 cores
	std::vector<struct lcp::core *> level3_cores;
	level3_cores.resize(3);

	ublock *p11 = new ublock[1];
	p11[0] = 0b00110111;
	level3_cores[0] = new lcp::core(8, p11, 1, 0, 0);
	ublock *p12 = new ublock[1];
	p12[0] = 0b0111101100;
	level3_cores[1] = new lcp::core(10, p12, 1, 0, 0);
	ublock *p13 = new ublock[1];
	p13[0] = 0b10110000101;
	level3_cores[2] = new lcp::core(11, p13, 1, 0, 0);

	// deepen to level 3
	success = lps_obj.deepen(3);
	assert(success && "Deepening to level 3 should be successful");

	assert(lps_obj.cores->size() == level3_cores.size() && "Core size at level 3 should match");
	for (size_t i = 0; i < lps_obj.cores->size(); i++) {
		assert((*(lps_obj.cores))[i] == *(level3_cores[i]) && "Cores at level 3 should match");
	}

	// cleanup level 3 cores
	for (size_t i = 0; i < level3_cores.size(); i++) {
		delete level3_cores[i];
	}

	// attempt to deepen to a lower level (should not do anything)
	success = lps_obj.deepen(2);
	assert(!success && "Deepening to a lower level should be unsuccessful");

	log("...  test_lps_deepen passed!");
}

int main() {

	log("Running test_lps...");

    test_lps_char_constructor();
    test_lps_char_rev_compl_constructor();
	test_lps_constructor();
	test_lps_file_io();
	test_lps_deepen();

	log("All tests in test_lps completed successfully!");

	return 0;
}
