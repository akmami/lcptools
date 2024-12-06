/**
 * @file encoding.h
 * @brief Provides utilities for encoding and reverse complementing DNA
 * sequences using a character-to-bit mapping system.
 *
 * This file defines functions and variables for managing the encoding of DNA
 * sequences into bit-encoded values, as well as their reverse complement
 * encodings. It includes functionality for initializing encoding coefficients,
 * displaying the encoding summary, and loading custom encodings from
 * user-specified files or maps.
 *
 * Key functionalities include:
 *   - Initializes encoding coefficients for standard DNA bases (A, C, G, T) and
 * their reverse complements.
 *   - Supports custom encoding initialization via a map or file input.
 *   - Automatically calculates dictionary bit size based on the maximum
 * encoding values.
 *   - Provides reverse complement encodings for DNA sequences, a critical
 * feature in bioinformatics.
 *   - Displays an overview of the encoding scheme including coefficients and
 * bit size.
 *   - Loads encoding mappings from an external file, making it easy to extend
 * the encoding system for custom alphabets or symbols.
 *
 * Example usage:
 * @code
 *   // Initialize standard encoding and reverse complements
 *   lcp::init_coefficients(true);
 *
 *   // Initialize encoding from a file
 *   std::string encoding_file = "dna_encoding.txt";
 *   lcp::init_coefficients(encoding_file, true);
 * @endcode
 *
 * @see constant.h
 * @see core.h
 *
 * @namespace lcp
 *
 * @note Initialization functions throw std::invalid_argument exceptions for
 * invalid maps or file formats.
 *
 * @author Akmuhammet Ashyralyyev
 * @version 1.0
 * @date 2024-09-14
 *
 */

#ifndef ENCODING_H
#define ENCODING_H

#include "constant.h"
#include <fstream>
#include <iostream>
#include <map>

namespace lcp {

	extern int alphabet[128];
	extern int rc_alphabet[128];
	extern char characters[128];
	extern int alphabet_bit_size;

	namespace encoding {

		/**
		 * @brief Displays the alphabet encoding summary including coefficients
		 * and dictionary bit size.
		 */
		void summary();

		/**
		 * @brief Initializes the encoding coefficients for standard DNA bases
		 * (A, C, G, T) and their reverse complements. Sets default values for
		 * coefficients and dictionary bit size.
		 * @param verbose If true, prints the encoding summary after
		 * initialization.
		 * @return Always returns 0 upon successful initialization.
		 */
		int init(bool verbose = LCP_VERBOSE);

		/**
		 * @brief Initializes the encoding coefficients and reverse complement
		 * coefficients using user-specified mappings. Calculates the dictionary
		 * bit size based on the maximum encoding value.
		 * @param map A mapping of characters to encoding values.
		 * @param rc_map A mapping of characters to reverse complement encoding
		 * values.
		 * @param verbose If true, prints the encoding summary after
		 * initialization.
		 * @return Always returns 0 upon successful initialization.
		 * @throws std::invalid_argument if any value in the map or reverse
		 * complement map is negative.
		 */
		int init(std::map<char, int> map, std::map<char, int> rc_map,
				 bool verbose = LCP_VERBOSE);

		/**
		 * @brief Initializes the encoding coefficients by reading them from a
		 * file. The file must contain character, encoding, and reverse
		 * complement encoding on each line.
		 * @param filename Path to the file containing the character encodings.
		 * @param verbose If true, prints the encoding summary after
		 * initialization.
		 * @return Always returns 0 upon successful initialization.
		 * @throws std::invalid_argument if any invalid data is found in the
		 * file.
		 */
		int init(std::string filename, bool verbose = LCP_VERBOSE);

	}; // namespace encoding

}; // namespace lcp

#endif
