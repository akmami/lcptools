/**
 * @file lps.h
 * @brief Defines the lps struct and its associated methods for handling locally consistent
 * parsing (LCP) of strings.
 *
 * The lps struct is responsible for performing LCP operations on input strings, constructing
 * cores, and supporting functionalities like parsing, compression (deepening), and memory
 * usage calculations. It includes methods for reading and writing the data to files as well
 * as deepening the LCP to higher levels of compression.
 *
 * The lps struct leverages various helper classes like core, encoding, and hash to manage the
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
 * @struct lps
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

#include "constant.h"
#include "core.h"
#include "encoding.h"
#include "rules.h"
#include <fstream>
#include <string>
#include <vector>

namespace lcp {

	struct lps {
	  public:
		int level;
		std::vector<struct core> *cores;

		/**
		 * @brief Constructor for the lps struct that processes a string by splitting it into segments and merging cores.
		 *
		 * This constructor divides the input string into segments of a specified size (defaulting to 1,000,000)
		 * and processes each segment individually. It also handles overlapping regions (defaulting to 10,000)
		 * between segments to ensure continuity. The processed segments are merged by matching cores and
		 * eliminating redundancy in the overlapping regions.
		 *
		 * @param str Reference to the input string to be processed.
		 * @param lcp_level The depth of processing for each core in the LCP structure.
		 * @param sequence_split_length (Optional) Length of each segment to split the string. Defaults to 1,000,000.
		 * @param overlap_margin_length (Optional) Length of the overlapping region between consecutive segments. Defaults to 10,000.
		 */
		lps(std::string &str, const int lcp_level, const size_t sequence_split_length = MAX_STR_LENGTH, const size_t overlap_margin_length = OVERLAP_MARGIN);

		/**
		 * @brief Constructor for the lps struct that processes a segment of a string.
		 *
		 * This constructor processes the part of the string defined by the range [begin, end).
		 * It reserves memory for the cores based on the size of the segment and performs parsing.
		 *
		 * @param begin Iterator pointing to the beginning of the segment.
		 * @param end Iterator pointing to the end of the segment.
		 */
		lps(std::string::iterator begin, std::string::iterator end);

		/**
		 * @brief Constructs an lps object from a string, with an option to apply reverse complement
		 * transformation.
		 *
		 * @param str The input string to be parsed.
		 * @param rev_comp Whether to apply reverse complement (default is false).
		 */
		lps(std::string &str, bool use_map = LCP_USE_MAP, bool rev_comp = LCP_REV_COMP);

		/**
		 * @brief Constructs an lps object by reading from a binary input file.
		 *
		 * @param in Input stream from which the lps data is read.
		 */
		lps(std::ifstream &in);

		/**
		 * @brief Destructor for the lps object. Frees dynamically allocated memory for cores.
		 */
		~lps();

		/**
		 * @brief Deepens the compression level of the LCP structure. This method compresses the
		 * existing cores and finds new cores.
		 *
		 * @return True if successful in deepening the structure, false otherwise.
		 */
		bool deepen(bool use_map = LCP_USE_MAP);

		/**
		 * @brief Deepens the compression level of the LCP structure to a specific level.
		 *
		 * @param lcp_level The target compression level to deepen to.
		 * @return True if deepening was successful, false otherwise.
		 */
		bool deepen(int lcp_level, bool use_map = LCP_USE_MAP);

		/**
		 * @brief Writes the current LCP structure to an existing output stream.
		 *
		 * @param out The output file stream to write to.
		 */
		void write(std::ofstream &out) const;

		/**
		 * @brief Calculates and returns the memory size used by the LCP structure.
		 *
		 * @return The memory size (in bytes) used by the LCP structure.
		 */
		double memsize() const;

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
		bool get_labels(std::vector<ulabel> &labels) const;

		/**
		 * @brief Returns the number of cores stored in the LPS structure.
		 *
		 * This function provides the size of the `cores` vector within the `lps` class,
		 * representing the total count of core segments currently stored.
		 *
		 * @return size_t Number of cores in the `lps` instance.
		 */
		size_t size() const;

	  private:
		/**
		 * @brief Performs Deterministic Coin Tossing (DCT) compression on binary sequences.
		 *
		 * This function is a central part of the LCP (Locally Consisted Parsing) algorithm. It identifies differences
		 * between consecutive binary strings, compressing the information by focusing on the position and value of
		 * the first divergent bit from the right-end of the strings. This difference is used to generate a compact
		 * 'core' that encapsulates the unique elements of each sequence in a smaller binary form.
		 *
		 * This compression significantly reduces redundant information, making further analysis of the sequences
		 * within the LCP framework more efficient and manageable.
		 *
		 * @return True if dct is performed, False if no enough cores are available for dct.
		 */
		bool dct();

		/**
		 * @brief Parses a sequence to extract Locally Consisted Parsing (LCP) cores and stores them in a vector.
		 *
		 * This function iterates over a sequence defined by iterators `begin` and `end` and identifies key
		 * segments, called "cores," that represent the (LCP) regions. By analyzing
		 * character relationships in the sequence (such as equality or relative order), it builds and stores
		 * these cores for further processing in the LCP framework.
		 *
		 * @tparam Iterator Type of the sequence iterator, supporting random access.
		 * @tparam Compare Type of comparison function, used to evaluate relationships between elements.
		 *
		 * @param begin Iterator pointing to the beginning of the sequence to parse.
		 * @param end Iterator pointing to the end of the sequence to parse.
		 * @param cores Pointer to a vector where the identified LCP cores will be stored.
		 * @param extension_size Size of the extension applied to core boundaries when calculating positions.
		 * @param gt Comparator for greater-than relations between sequence elements.
		 * @param lt Comparator for less-than relations between sequence elements.
		 * @param eq Comparator for equality relations between sequence elements.
		 * @param use_map Boolean flag indicating whether a map (hash table) should be used in the `core` structure.
		 *
		 * @details
		 * The function traverses the sequence, analyzing consecutive elements to locate boundaries of LCP
		 * regions. It checks for various conditions:
		 * - **Middle count**: Counts contiguous elements that are equal, which could form a central core if
		 *   they span more than one element.
		 * - **Local minimum (LMIN) and maximum (LMAX)**: Identifies positions where the sequence is at a
		 *   local minimum or maximum, signifying core boundaries.
		 *
		 * @note `use_map` influences how cores are constructed.
		 *
		 */
		template <typename Iterator, typename Compare, typename Index, typename Size, typename Representation, typename Data>
		inline void parse(Iterator begin, Iterator end, std::vector<struct core> *cores, const size_t extension_size, Compare gt, Compare lt, Compare eq, Index fn_index, Size fn_size, Representation fn_rep, Data fn_data, bool use_map) {

			Iterator it1 = begin + extension_size;
			Iterator it2 = end;

			// find lcp cores
			for (; it1 + 2 < end; it1++) {

				// skip invalid character
				if (eq(it1, it1 + 1)) {
					continue;
				}

				size_t middleCount = countMiddle(it1, end, eq);

				if (middleCount > 1) {

					if (isSSEQ(it1, it2)) {
						cores->emplace_back(it2 - 1 - extension_size, it1 + 1, fn_index(begin, it2 - 1 - extension_size, it1 + 1), fn_size, fn_rep, fn_data, use_map);
					}

					it2 = it1 + 2 + middleCount;
					cores->emplace_back(it1 - extension_size, it2, fn_index(begin, it1 - extension_size, it2), fn_size, fn_rep, fn_data, use_map);

					continue;
				}

				if (isLMIN(it1, gt, lt)) {

					if (isSSEQ(it1, it2)) {
						cores->emplace_back(it2 - 1 - extension_size, it1 + 1, fn_index(begin, it2 - 1 - extension_size, it1 + 1), fn_size, fn_rep, fn_data, use_map);
					}
					it2 = it1 + 3;
					cores->emplace_back(it1 - extension_size, it2, fn_index(begin, it1 - extension_size, it2), fn_size, fn_rep, fn_data, use_map);

					continue;
				}

				if (begin == it1) {
					continue;
				}

				if (isLMAX(it1, end, gt, lt)) {

					if (isSSEQ(it1, it2)) {
						cores->emplace_back(it2 - 1 - extension_size, it1 + 1, fn_index(begin, it2 - 1 - extension_size, it1 + 1), fn_size, fn_rep, fn_data, use_map);
					}
					it2 = it1 + 3;
					cores->emplace_back(it1 - extension_size, it2, fn_index(begin, it1 - extension_size, it2), fn_size, fn_rep, fn_data, use_map);

					continue;
				}
			}
		};
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
	std::ostream &operator<<(std::ostream &os, const lps &element);

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
	std::ostream &operator<<(std::ostream &os, const lps *element);

	/**
	 * @brief Equality operator for comparing two lcp::lps objects.
	 *
	 * This function compares the sizes of the core vectors of both objects, and then
	 * compares each core element by dereferencing the core pointers. If all elements match,
	 * the function returns true; otherwise, it returns false.
	 *
	 * @param lhs The left-hand side lps object to compare.
	 * @param rhs The right-hand side lps object to compare.
	 * @return true if both lps objects are equal, false otherwise.
	 */
	bool operator==(const lcp::lps &lhs, const lcp::lps &rhs);

	/**
	 * @brief Inequality operator for comparing two lcp::lps objects.
	 *
	 * This function first checks if the sizes of the core vectors are different.
	 * If they are, the objects are not equal. It then checks each core element.
	 * If any element is different, the function returns true; otherwise, it returns false.
	 *
	 * @param lhs The left-hand side lps object to compare.
	 * @param rhs The right-hand side lps object to compare.
	 * @return true if the lps objects are not equal, false otherwise.
	 */
	bool operator!=(const lcp::lps &lhs, const lcp::lps &rhs);
}; // namespace lcp

#endif
