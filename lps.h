/**
 * @file lps.h
 * @brief Defines the lps class and its associated methods for handling locally consistent 
 * parsing (LCP) of strings.
 *
 * The lps class is responsible for performing LCP operations on input strings, constructing 
 * cores, and supporting functionalities like parsing, compression (deepening), and memory 
 * usage calculations. It includes methods for reading and writing the data to files as well 
 * as deepening the LCP to higher levels of compression.
 *
 * The lps class leverages various helper classes like core, encoding, and hash to manage the 
 * string data and its compressed forms. Additionally, it supports both standard and 
 * reverse-complement parsing for specialized string handling in bioinformatics and other fields.
 *
 * Key functionalities include:
 * - Parsing an input string or file to extract LCP cores.
 * - Performing multi-level compression of LCP cores (deepening).
 * - Saving and loading LCP cores from files.
 * - Calculating memory usage of the constructed LCP structure.
 * 
 * Dependencies:
 * - Requires core.h, encoding.h, hash.h, and constant.h for auxiliary data structures and utilities.
 *
 * Example usage:
 * @code
 *   std::string sequence = "AGCTAGCTAG";
 *   lcp::lps parser(sequence);
 *   parser.deepen();
 *   parser.write("output.lps");
 * @endcode
 * 
 * @see core.h
 * @see encoding.h
 * @see hash.h
 * @see constant.h
 *
 * @namespace lcp
 * @class lps
 * 
 * @note Destructor handles clean-up of allocated memory for cores.
 * 
 * @author Akmuhammet Ashyralyyev
 * @version 1.0
 * @date 2024-09-14
 * 
 */

#ifndef LPS_H
#define LPS_H

#include <cstdint>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "constant.h"
#include "encoding.h"
#include "core.h"
#include "hash.h"


namespace lcp {

    class lps {
    public:
        int level;
        std::vector<core*> *cores;

        /**
         * @brief Constructs an lps object from a string, with an option to apply reverse complement 
         * transformation.
         * 
         * @param str The input string to be parsed.
         * @param rev_comp Whether to apply reverse complement (default is false).
         */
        lps(std::string &str, bool rev_comp = false);

        /**
         * @brief Constructs an lps object by reading from a binary input file.
         * 
         * @param in Input stream from which the lps data is read.
         */
        lps(std::ifstream& in);

        /**
         * @brief Destructor for the lps object. Frees dynamically allocated memory for cores.
         */
        ~lps();


        /**
         * @brief Parses the input string range and populates the LCP cores. This method handles 
         * reverse complement and validates substrings to identify and extract cores.
         * 
         * @param begin Iterator pointing to the start of the string range to parse.
         * @param end Iterator pointing to the end of the string range to parse.
         * @param rev_comp Whether to apply reverse complement parsing.
         */
        void parse(std::string::iterator begin, std::string::iterator end, bool rev_comp = false);

        /**
         * @brief Deepens the compression level of the LCP structure. This method compresses the 
         * existing cores and finds new cores.
         * 
         * @return True if successful in deepening the structure, false otherwise.
         */
        bool deepen();

        /**
         * @brief Deepens the compression level of the LCP structure to a specific level.
         * 
         * @param lcp_level The target compression level to deepen to.
         * @return True if deepening was successful, false otherwise.
         */
        bool deepen(int lcp_level);

        /**
         * @brief Writes the current LCP structure to a binary file with the specified filename.
         * 
         * @param filename The name of the output file to write to.
         */
        void write(std::string filename) const;

        /**
         * @brief Writes the current LCP structure to an existing output stream.
         * 
         * @param out The output file stream to write to.
         */
        void write(std::ofstream& out) const;

        /**
         * @brief Calculates and returns the memory size used by the LCP structure.
         * 
         * @return The memory size (in bytes) used by the LCP structure.
         */
        double memsize();

        /**
         * @brief Sets the count of cores in the provided core_count vector.
         *
         * This function checks if the reverse_map is empty. If it is empty,
         * the function returns false, indicating that no core counts were set.
         * Otherwise, it iterates through the `cores` vector and updates the core
         * count based on the label of each core by calling the `count_core` function.
         *
         * @param core_count A reference to a vector of unsigned integers representing the count of cores.
         * @return true if the core count was successfully set, false if the reverse_map is empty.
         */
        bool set_core_count( std::vector<uint32_t>& core_counts );

        /**
         * @brief Updates the core count in the provided core_count vector.
         *
         * This function iterates through the `cores` vector and increments
         * the corresponding value in the `core_count` vector based on the label
         * of each core. Each core's label is used as an index in the `core_count`
         * vector, and the count for that label is incremented by 1.
         *
         * @param core_count A reference to a vector of unsigned integers where each element
         *                   represents the count of cores for the corresponding label.
         * @return true Always returns true after updating the core counts.
         */
        bool update_core_count( std::vector<uint32_t>& core_counts );

        /**
         * @brief Retrieves the labels of all cores and stores them in the provided labels vector.
         *
         * This function reserves space in the `labels` vector based on the size of the `cores` vector 
         * and initial size of itself and iterates through the `cores` vector, adding the label of 
         * each core to the `labels` vector.
         *
         * @param labels A reference to a vector of unsigned integers where the labels of the cores will be stored.
         * @return true if the labels were successfully retrieved.
         */
        bool get_labels(std::vector<uint32_t>& labels);
    };

    /**
	 * @brief Overloads the stream insertion operator (<<) to output the representation of a `lcp` object.
	 *
	 * This function iterates over the cores in the `cores` vector and outputs them to the given output stream. 
	 * It prints each cores as bit representation, after initially printing the LCP level.
	 *
	 * @param os The output stream to which the cores of the `cores` object will be written.
	 * @param element A `lps` object to be output.
	 * @return A reference to the output stream `os` after the `lps` object has been written.
	 */
    std::ostream& operator<<(std::ostream& os, const lps& element);

    /**
	 * @brief Overloads the stream insertion operator (<<) to output the representation of a `lcp` pointer.
	 *
	 * This function iterates over the cores in the `cores` vector and outputs them to the given output stream. 
	 * It prints each cores as bit representation, after initially printing the LCP level.
	 *
	 * @param os The output stream to which the cores of the `cores` object will be written.
	 * @param element A pointer to the `lps` object to be output.
	 * @return A reference to the output stream `os` after the `lps` object has been written.
	 */
    std::ostream& operator<<(std::ostream& os, const lps* element);
};

#endif
