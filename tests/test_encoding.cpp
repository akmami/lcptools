#include "encoding.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>

void log(const std::string &message) {
	std::cout << message << std::endl;
};

void test_encoding_initialization_default() {

	lcp::encoding::init(false);

	// check default alphabet
	assert(lcp::alphabet['A'] == 0 && "A should be encoded as 0");
	assert(lcp::alphabet['C'] == 1 && "C should be encoded as 1");
	assert(lcp::alphabet['G'] == 2 && "G should be encoded as 2");
	assert(lcp::alphabet['T'] == 3 && "T should be encoded as 3");
	assert(lcp::alphabet['a'] == 0 && "a should be encoded as 0");
	assert(lcp::alphabet['c'] == 1 && "c should be encoded as 1");
	assert(lcp::alphabet['g'] == 2 && "g should be encoded as 2");
	assert(lcp::alphabet['t'] == 3 && "t should be encoded as 3");

	// check reverse complement alphabet
	assert(lcp::rc_alphabet['A'] == 3 && "Reverse complement of A should be 3");
	assert(lcp::rc_alphabet['C'] == 2 && "Reverse complement of C should be 2");
	assert(lcp::rc_alphabet['G'] == 1 && "Reverse complement of G should be 1");
	assert(lcp::rc_alphabet['T'] == 0 && "Reverse complement of T should be 0");
	assert(lcp::rc_alphabet['a'] == 3 && "Reverse complement of a should be 3");
	assert(lcp::rc_alphabet['c'] == 2 && "Reverse complement of c should be 2");
	assert(lcp::rc_alphabet['g'] == 1 && "Reverse complement of g should be 1");
	assert(lcp::rc_alphabet['t'] == 0 && "Reverse complement of t should be 0");

	// check dictionary bit size
	assert(lcp::alphabet_bit_size == 2 && "Alphabet bit size should be 2");

	log("...  test_encoding_initialization_default passed!");
};

void test_encoding_initialization_from_map() {

	std::map<char, int> map = {{'A', 2}, {'C', 3}, {'G', 0}, {'T', 4}};
	std::map<char, int> rc_map = {{'A', 4}, {'C', 0}, {'G', 3}, {'T', 2}};

	lcp::encoding::init(map, rc_map, false);

	// check coefficients
	assert(lcp::alphabet['A'] == 2 && "A should be encoded as 2");
	assert(lcp::alphabet['C'] == 3 && "C should be encoded as 3");
	assert(lcp::alphabet['G'] == 0 && "G should be encoded as 0");
	assert(lcp::alphabet['T'] == 4 && "T should be encoded as 4");

	// check reverse complement coefficients
	assert(lcp::rc_alphabet['A'] == 4 && "Reverse complement of A should be 4");
	assert(lcp::rc_alphabet['C'] == 0 && "Reverse complement of C should be 0");
	assert(lcp::rc_alphabet['G'] == 3 && "Reverse complement of G should be 3");
	assert(lcp::rc_alphabet['T'] == 2 && "Reverse complement of T should be 2");

	// check dictionary bit size
	assert(lcp::alphabet_bit_size == 3 && "Alphabet bit size should be 3");

	log("...  test_encoding_initialization_from_map passed!");
};

void test_encoding_initialization_from_file() {

	// create a temporary encoding file
	std::ofstream encoding_file("encoding_test.txt");
	encoding_file << "A 5 2\n";
	encoding_file << "C 3 3\n";
	encoding_file << "G 7 0\n";
	encoding_file << "T 8 1\n";
	encoding_file.close();

	lcp::encoding::init("encoding_test.txt", false);

	// check alphabet
	assert(lcp::alphabet['A'] == 5 && "A should be encoded as 5");
	assert(lcp::alphabet['C'] == 3 && "C should be encoded as 3");
	assert(lcp::alphabet['G'] == 7 && "G should be encoded as 7");
	assert(lcp::alphabet['T'] == 8 && "T should be encoded as 8");

	// check reverse complement alphabet
	assert(lcp::rc_alphabet['A'] == 2 && "Reverse complement of A should be 2");
	assert(lcp::rc_alphabet['C'] == 3 && "Reverse complement of C should be 3");
	assert(lcp::rc_alphabet['G'] == 0 && "Reverse complement of G should be 0");
	assert(lcp::rc_alphabet['T'] == 1 && "Reverse complement of T should be 1");

	// check dictionary bit size
	assert(lcp::alphabet_bit_size == 4 && "Alphabet bit size should be 4");

	// clean up the temporary file
	std::remove("encoding_test.txt");

	log("...  test_encoding_initialization_from_file passed!");
};

int main() {
	log("Running test_encoding...");

	test_encoding_initialization_default();
	test_encoding_initialization_from_map();
	test_encoding_initialization_from_file();

	log("All tests in test_encoding completed successfully!");

	return 0;
};
