#ifndef RULES_H
#define RULES_H

#include "core.h"
#include <string>
#include <vector>

namespace lcp {

	// MARK: Properties

	/**
	 * @brief Computes the indices of two char pointers relative to the beginning of the string.
	 *
	 * Calculates the distance of two char pointers (`it1` and `it2`) from the starting char pointer (`begin`).
	 * Returns the indices as a pair.
	 *
	 * @param begin The char pointer pointing to the beginning of the string.
	 * @param it1 The first pointer whose index is to be computed.
	 * @param it2 The second pointer whose index is to be computed.
	 * @return A pair of indices representing the positions of `it1` and `it2` relative to `begin`.
	 */
	inline std::pair<size_t, size_t> char_ptr_index(const char *begin, const char *it1, const char *it2) {
		return std::make_pair(std::distance(begin, it1), std::distance(begin, it2));
	};

	/**
	 * @brief Computes the indices of two string iterators relative to the beginning of the string.
	 *
	 * Calculates the distance of two iterators (`it1` and `it2`) from the starting iterator (`begin`).
	 * Returns the indices as a pair.
	 *
	 * @param begin The iterator pointing to the beginning of the string.
	 * @param it1 The first iterator whose index is to be computed.
	 * @param it2 The second iterator whose index is to be computed.
	 * @return A pair of indices representing the positions of `it1` and `it2` relative to `begin`.
	 */
	inline std::pair<size_t, size_t> char_index(std::string::iterator begin, std::string::iterator it1, std::string::iterator it2) {
		return std::make_pair(std::distance(begin, it1), std::distance(begin, it2));
	};

	/**
	 * @brief Computes the indices of two core iterators relative to the beginning of the vector or their start/end values.
	 *
	 * Calculates the distance of two iterators (`it1` and `it2`) from the starting iterator (`begin`).
	 * If the `STATS` macro is defined, the indices are taken as the `start` and `end` values from the respective `core` objects.
	 *
	 * @param begin The iterator pointing to the beginning of the vector of cores.
	 * @param it1 The first iterator whose index is to be computed.
	 * @param it2 The second iterator whose index is to be computed.
	 * @return A pair of indices representing the positions of `it1` and `it2` relative to `begin`,
	 *         or their `start` and `end` values if `STATS` is defined.
	 */
	inline std::pair<size_t, size_t> core_index(std::vector<struct core>::iterator begin, std::vector<struct core>::iterator it1, std::vector<struct core>::iterator it2) {
		(void)begin;
#ifdef STATS
		return std::make_pair(it1->start, (it2 - 1)->end);
#endif
		return std::make_pair(std::distance(begin, it1), std::distance(begin, it2));
	};

    /**
	 * Gets the bit length of the character encoding that is given in char pointer.
	 *
	 * @param it A char pointer pointing to the character. It doesn't matter which character is given.
	 *          It will return alphabet_bit_size value.
	 * @return A bit length of the encoding of the given character (i.e. alphabet_bit_size)
	 */
	inline uint64_t char_ptr_size(const char *it) {
		(void)it;
		return alphabet_bit_size;
	};

	/**
	 * Gets the bit length of the character encoding that is given in string iterator.
	 *
	 * @param it An iterator pointing to the character. It doesn't matter which character is given.
	 *          It will return alphabet_bit_size value.
	 * @return A bit length of the encoding of the given character (i.e. alphabet_bit_size)
	 */
	inline uint64_t char_size(std::string::iterator it) {
		(void)it;
		return alphabet_bit_size;
	};

	/**
	 * Gets the bit length of the core representation that is given in vector iterator.
	 *
	 * @param it An iterator pointing to the core object.
	 * @return A bit length of the core object.
	 */
	inline uint64_t core_size(std::vector<struct core>::iterator it) {
		return it->bit_size;
	};

    /**
	 * Gets the character encoding from the alphabet.
	 *
	 * @param it A char pointer pointing to the character.
	 * @return An encoding of the character.
	 */
	inline ublock *char_ptr_rep(const char *it) {
		thread_local static ublock temp;
		temp = static_cast<ublock>(alphabet[static_cast<unsigned char>(*it)]);
		return &temp;
	};

	/**
	 * Gets the character encoding from the alphabet.
	 *
	 * @param it An iterator pointing to the character.
	 * @return An encoding of the character.
	 */
	inline ublock *char_rep(std::string::iterator it) {
		thread_local static ublock temp;
		temp = static_cast<ublock>(alphabet[static_cast<unsigned char>(*it)]);
		return &temp;
	};

	/**
	 * Gets the character encoding from the reverse complement alphabet.
	 *
	 * @param it A char pointer pointing to the character.
	 * @return An encoding of the character.
	 */
	inline ublock *char_ptr_rev_rep(char *it) {
		thread_local static ublock temp;
		temp = static_cast<ublock>(rc_alphabet[static_cast<unsigned char>(*it)]);
		return &temp;
	};

	/**
	 * Gets the character encoding from the reverse complement alphabet.
	 *
	 * @param it An iterator pointing to the character.
	 * @return An encoding of the character.
	 */
	inline ublock *char_rev_rep(std::string::iterator it) {
		thread_local static ublock temp;
		temp = static_cast<ublock>(rc_alphabet[static_cast<unsigned char>(*it)]);
		return &temp;
	};

	/**
	 * Gets the Core object's representation.
	 *
	 * @param it An iterator pointing to the Core object.
	 * @return An representation of the Core object.
	 */
	inline ublock *core_rep(std::vector<struct core>::iterator it) {
		return it->bit_rep;
	};

	/**
	 * @brief Extracts character data from a range of char pointers and returns it as a dynamically allocated array.
	 *
	 * Copies characters from the range specified by the `begin` and `end` pointers into a newly allocated `char` array.
	 * The caller is responsible for deleting the returned array.
	 *
	 * @param begin A char pointer pointing to the start of the string range.
	 * @param end A char pointer pointing to the end of the string range.
	 * @return A pointer to a dynamically allocated array containing the characters in the specified range.
	 */
	inline ulabel char_ptr_data(const char *begin, const char *end) {
		thread_local static int double_shift = 2 * alphabet_bit_size;
		thread_local static int triple_shift = 3 * alphabet_bit_size;
		thread_local static ulabel data;
		data = 0;
		data |= ((std::distance(begin,end)-2) << triple_shift);
		data |= (alphabet[(*(begin)) & 0xDF] << double_shift);
		data |= (alphabet[(*(end-2)) & 0xDF] << alphabet_bit_size);
		data |= (alphabet[(*(end-1)) & 0xDF]);
		return data;
	};

	/**
	 * @brief Extracts character data from a range of string iterators and returns it as a dynamically allocated array.
	 *
	 * Copies characters from the range specified by the `begin` and `end` iterators into a newly allocated `char` array.
	 * The caller is responsible for deleting the returned array.
	 *
	 * @param begin An iterator pointing to the start of the string range.
	 * @param end An iterator pointing to the end of the string range.
	 * @return A pointer to a dynamically allocated array containing the characters in the specified range.
	 */
	inline ulabel char_data(const std::string::iterator begin, const std::string::iterator end) {
		thread_local static int double_shift = 2 * alphabet_bit_size;
		thread_local static int triple_shift = 3 * alphabet_bit_size;
		thread_local static ulabel data;
		data = 0;
		data |= ((std::distance(begin,end)-2) << triple_shift);
		data |= (alphabet[(*(begin)) & 0xDF] << double_shift);
		data |= (alphabet[(*(end-2)) & 0xDF] << alphabet_bit_size);
		data |= (alphabet[(*(end-1)) & 0xDF]);
		return data;
	};

	/**
	 * @brief Extracts core data from a range of core iterators and returns it as a dynamically allocated array.
	 *
	 * Copies `DCT_ITERATION_COUNT` core labels from the beginning of the specified range, followed by the labels of
	 * the last two cores in the range, and finally adds the length of the range minus `DCT_ITERATION_COUNT` and two.
	 * The caller is responsible for deleting the returned array.
	 *
	 * @param begin An iterator pointing to the start of the core range.
	 * @param end An iterator pointing to the end of the core range.
	 * @return A pointer to a dynamically allocated array containing the core data.
	 */
	inline ulabel *core_data(const std::vector<struct core>::iterator begin, const std::vector<struct core>::iterator end) {
		thread_local static ulabel data[4];
		data[0] = (begin+DCT_ITERATION_COUNT)->label;
		data[1] = (end-2)->label;
		data[2] = (end-1)->label;
		data[3] = std::distance(begin,end)-DCT_ITERATION_COUNT-2;
		return data;
	};

	// MARK: Operators

	/**
	 * Compares two characters from a string using a custom alphabet mapping.
	 *
	 * @param it1 A char pointer pointing to the first character.
	 * @param it2 A char pointer pointing to the second character.
	 * @return true if the character pointed to by it1 is greater than the character
	 *         pointed to by it2 based on the modularity of the alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_ptr_gt(const char *it1, const char *it2) {
		return (alphabet[static_cast<unsigned char>(*it1)]) > (alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping.
	 *
	 * @param it1 An iterator pointing to the first character.
	 * @param it2 An iterator pointing to the second character.
	 * @return true if the character pointed to by it1 is greater than the character
	 *         pointed to by it2 based on the modularity of the alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_gt(const std::string::iterator it1, const std::string::iterator it2) {
		return (alphabet[static_cast<unsigned char>(*it1)]) > (alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping.
	 *
	 * @param it1 A char pointer pointing to the first character.
	 * @param it2 A char pointer pointing to the second character.
	 * @return true if the character pointed to by it1 is less than the character
	 *         pointed to by it2 based on the modularity of the alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_ptr_lt(const char *it1, const char *it2) {
		return (alphabet[static_cast<unsigned char>(*it1)]) < (alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping.
	 *
	 * @param it1 An iterator pointing to the first character.
	 * @param it2 An iterator pointing to the second character.
	 * @return true if the character pointed to by it1 is less than the character
	 *         pointed to by it2 based on the modularity of the alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_lt(const std::string::iterator it1, const std::string::iterator it2) {
		return (alphabet[static_cast<unsigned char>(*it1)]) < (alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping.
	 *
	 * @param it1 A char pointer pointing to the first character.
	 * @param it2 A char pointer pointing to the second character.
	 * @return true if the character pointed to by it1 is equal to the character
	 *         pointed to by it2 based on the modularity of the alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_ptr_eq(const char *it1, const char *it2) {
		return (alphabet[static_cast<unsigned char>(*it1)]) == (alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping.
	 *
	 * @param it1 An iterator pointing to the first character.
	 * @param it2 An iterator pointing to the second character.
	 * @return true if the character pointed to by it1 is equal to the character
	 *         pointed to by it2 based on the modularity of the alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_eq(const std::string::iterator it1, const std::string::iterator it2) {
		return (alphabet[static_cast<unsigned char>(*it1)]) == (alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two core objects pointed to by iterators to determine their order.
	 *
	 * @param it1 An iterator pointing to the first core object.
	 * @param it2 An iterator pointing to the second core object.
	 * @return true if the core object pointed to by it1 is greater than the core
	 *         object pointed to by it2; false otherwise.
	 */
	inline bool core_gt(const std::vector<struct core>::iterator it1, const std::vector<struct core>::iterator it2) {
		return (*it1) > (*it2);
	};

	/**
	 * Compares two core objects pointed to by iterators to determine their order.
	 *
	 * @param it1 An iterator pointing to the first core object.
	 * @param it2 An iterator pointing to the second core object.
	 * @return true if the core object pointed to by it1 is less than the core
	 *         object pointed to by it2; false otherwise.
	 */
	inline bool core_lt(const std::vector<struct core>::iterator it1, const std::vector<struct core>::iterator it2) {
		return (*it1) < (*it2);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping.
	 *
	 * @param it1 An iterator pointing to the first character.
	 * @param it2 An iterator pointing to the second character.
	 * @return true if the character pointed to by it1 is equal to the character
	 *         pointed to by it2 based on the modularity of the alphabet mapping;
	 *         false otherwise.
	 */
	inline bool core_eq(const std::vector<struct core>::iterator it1, const std::vector<struct core>::iterator it2) {
		return (*it1) == (*it2);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping (reverse complement).
	 *
	 * @param it1 A char pointer pointing to the first character.
	 * @param it2 A char pointer pointing to the second character.
	 * @return true if the character pointed to by it1 is greater than the character
	 *         pointed to by it2 based on the modularity of the reverse complement alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_ptr_rc_gt(const char *it1, const char *it2) {
		return (rc_alphabet[static_cast<unsigned char>(*it1)]) > (rc_alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping (reverse complement).
	 *
	 * @param it1 An iterator pointing to the first character.
	 * @param it2 An iterator pointing to the second character.
	 * @return true if the character pointed to by it1 is greater than the character
	 *         pointed to by it2 based on the modularity of the reverse complement alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_rc_gt(const std::string::iterator it1, const std::string::iterator it2) {
		return (rc_alphabet[static_cast<unsigned char>(*it1)]) > (rc_alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping (reverse complement).
	 *
	 * @param it1 A char pointer pointing to the first character.
	 * @param it2 A char pointer pointing to the second character.
	 * @return true if the character pointed to by it1 is less than the character
	 *         pointed to by it2 based on the modularity of the reverse complement alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_ptr_rc_lt(const char *it1, const char *it2) {
		return (rc_alphabet[static_cast<unsigned char>(*it1)]) < (rc_alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping (reverse complement).
	 *
	 * @param it1 An iterator pointing to the first character.
	 * @param it2 An iterator pointing to the second character.
	 * @return true if the character pointed to by it1 is less than the character
	 *         pointed to by it2 based on the modularity of the reverse complement alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_rc_lt(const std::string::iterator it1, const std::string::iterator it2) {
		return (rc_alphabet[static_cast<unsigned char>(*it1)]) < (rc_alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping (reverse complement).
	 *
	 * @param it1 A char pointer pointing to the first character.
	 * @param it2 A char pointer pointing to the second character.
	 * @return true if the character pointed to by it1 is equal to the character
	 *         pointed to by it2 based on the modularity of the reverse complement alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_ptr_rc_eq(const char *it1, const char *it2) {
		return (rc_alphabet[static_cast<unsigned char>(*it1)]) == (rc_alphabet[static_cast<unsigned char>(*it2)]);
	};

	/**
	 * Compares two characters from a string using a custom alphabet mapping (reverse complement).
	 *
	 * @param it1 An iterator pointing to the first character.
	 * @param it2 An iterator pointing to the second character.
	 * @return true if the character pointed to by it1 is equal to the character
	 *         pointed to by it2 based on the modularity of the reverse complement alphabet mapping;
	 *         false otherwise.
	 */
	inline bool char_rc_eq(const std::string::iterator it1, const std::string::iterator it2) {
		return (rc_alphabet[static_cast<unsigned char>(*it1)]) == (rc_alphabet[static_cast<unsigned char>(*it2)]);
	};

	// MARK: LCP rules

	/**
	 * Determines if the element at the given iterator position is a local minimum.
	 *
	 * @tparam Iterator The type of the iterator.
	 * @tparam Compare A comparison function, either greater-than or less-than.
	 * @param it The iterator pointing to the current element.
	 * @param gt The greater-than comparison function.
	 * @param lt The less-than comparison function.
	 * @return true if the element at the iterator position is a local minimum based
	 *         on the comparison functions; false otherwise.
	 */
	template <typename Iterator, typename Compare>
	bool isLMIN(const Iterator it, Compare gt, Compare lt) {
		return gt(it, it + 1) &&
			   lt(it + 1, it + 2);
	};

	/**
	 * Determines if the element at the given iterator position is a local maximum.
	 *
	 * @tparam Iterator The type of the iterator.
	 * @tparam Compare A comparison function, either greater-than or less-than.
	 * @param it The iterator pointing to the current element.
	 * @param end The end iterator for the range being checked.
	 * @param gt The greater-than comparison function.
	 * @param lt The less-than comparison function.
	 * @return true if the element at the iterator position is a local maximum based
	 *         on the comparison functions; false otherwise.
	 */
	template <typename Iterator, typename Compare>
	bool isLMAX(const Iterator it, const Iterator end, Compare gt, Compare lt) {
		return (it + 3) < end &&
			   lt(it, it + 1) &&
			   gt(it + 1, it + 2) &&
			   !gt(it - 1, it) &&
			   !lt(it + 2, it + 3);
	};

	/**
	 * Counts the number of consecutive elements in the middle of a sequence that are equal.
	 *
	 * @tparam Iterator The type of the iterator.
	 * @tparam Compare The equality comparison function.
	 * @param it The iterator pointing to the start of the range.
	 * @param end The end iterator for the range being checked.
	 * @param eq The equality comparison function.
	 * @return The count of consecutive elements in the middle that are equal; returns 0 if
	 *         the sequence runs to the end.
	 */
	template <typename Iterator, typename Compare>
	size_t countMiddle(const Iterator it, const Iterator end, Compare eq) {
		size_t middle_count = 1;
		Iterator temp = it + 2;
		while (temp < end && eq((temp - 1), temp)) {
			temp++;
			middle_count++;
		}
		if (temp == end) {
			return 0;
		}
		return middle_count;
	};

	/**
	 * Checks if two sequences do not overlap, indicating that one sequence starts
	 * after the other sequence ends.
	 *
	 * @tparam Iterator The type of the iterator.
	 * @param cur_begin The iterator pointing to the start of the current sequence.
	 * @param prev_end The iterator pointing to the end of the previous sequence.
	 * @return true if the sequences do not overlap (i.e., `cur_begin` starts after
	 *         `prev_end` ends); false otherwise.
	 */
	template <typename Iterator>
	bool isSSEQ(const Iterator cur_begin, const Iterator prev_end) {
		return prev_end < cur_begin;
	};

}; // namespace lcp

#endif