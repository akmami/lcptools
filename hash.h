/**
 * @file hash.h
 * @brief Defines hashing and equality structures for custom hashing of arrays, as well as 
 *        functions for managing hash tables in the LCP parsing system.
 *
 * This file provides the implementation of a custom hash function for arrays of three unsigned integers 
 * and an equality operator to be used in unordered maps. It also includes functions to initialize the hash 
 * maps for strings and cores, and to calculate hash values for byte sequences.
 *
 * Key functionalities include:
 *   - A specialized hash function for arrays of three unsigned integers, using a mixing algorithm 
 *     for robust hash generation.
 *   - An equality operator to compare arrays of three unsigned integers.
 *   - Functions to initialize hash tables (`str_map` and `core_map`) with preallocated sizes.
 *   - A function to hash a sequence of bytes from a string iterator range, using a seed value for initialization.
 * 
 * Example usage:
 * @code
 *   lcp::init_hashing(); // Initializes hash tables with default sizes.
 *   
 *   std::string sequence = "ACGT";
 *   size_t hash_value = lcp::hash_bytes(sequence.begin(), sequence.end());
 * @endcode
 * 
 * @see constant.h
 * 
 * @namespace lcp
 * 
 * @author Akmuhammet Ashyralyyev
 * @version 1.0
 * @date 2024-09-14
 * 
 */

#ifndef HASH_H
#define HASH_H

#include <unordered_map>
#include <string>
#include <vector>
#include <iterator>
#include <cstddef>
#include <cstring>
#include "lps.h"
#include "constant.h"


#define M       0x5bd1e995;
#define SEED    0x153ac45c;
#define MAP_KEY_VECTOR

struct cores {
    uint core1;
    uint core2;
    uint core3;
    uint middle_count;
};


struct hashing_cores {
    std::size_t operator() ( const struct cores& vec ) const {
        const size_t m = 0x5bd1e995;
        std::size_t hash = 0x153ac45c ^ 3;
        size_t chars;

        chars = vec.core1;
        chars *= m;
        chars ^= chars >> 24;
        chars *= m;
        hash *= m;
        hash ^= chars;

        // for( uint i = 0; i < vec.middle_count; i++ ) {
            chars = vec.core2;
            chars *= m;
            chars ^= chars >> 24;
            chars *= m;
            hash *= m;
            hash ^= chars;
        // }

        chars = vec.core3;
        chars *= m;
        chars ^= chars >> 24;
        chars *= m;
        hash *= m;
        hash ^= chars;

        // do a few final mixes of the hash.
        hash ^= hash >> 13;
        hash *= m;
        hash ^= hash >> 15;
        return hash;
    };
};


struct equality_cores {
    bool operator() ( const struct cores& lhs, const struct cores& rhs ) const {
        return lhs.core1 == rhs.core1 && lhs.core2 == rhs.core2 && lhs.core3 == rhs.core3 && lhs.middle_count == rhs.middle_count;
    };
};


struct hashing_vector {
    // std::size_t operator() ( const std::array<uint, 3>& arr ) const {
    std::size_t operator() ( const std::vector<uint>& arr ) const {
        const size_t m = 0x5bd1e995;
        size_t seed = 0x153ac45c;
        std::size_t hash = seed ^ 3;
        size_t chars;

        for(size_t i = 0; i < arr.size(); i++ ) {
            // hash element
            chars = arr[i];
            chars *= m;
            chars ^= chars >> 24;
            chars *= m;
            hash *= m;
            hash ^= chars;
        }

        // do a few final mixes of the hash.
        hash ^= hash >> 13;
        hash *= m;
        hash ^= hash >> 15;
        return hash;
    };
};


struct equality_vector {
    bool operator() ( const std::vector<uint>& lhs, const struct std::vector<uint>& rhs ) const {
        if ( lhs.size() != rhs.size() ) {
            return false;
        }

        for(size_t i = 0; i < lhs.size(); i++ ) {
            if ( lhs[i] != rhs[i] ) {
                return false;
            }
        }
        return true;
    };
};


#ifdef MAP_KEY_VECTOR
using core_map_key_type = std::vector<uint>;
using core_map_type = std::unordered_map<core_map_key_type, uint, hashing_vector, equality_vector>;
#else
using core_map_key_type = struct cores;
using core_map_type = std::unordered_map<core_map_key_type, uint, hashing_cores, equality_cores>;
#endif

namespace lcp {

    extern std::unordered_map<std::string, uint> str_map;
    extern core_map_type core_map;
    extern std::vector<const core_map_key_type*> reverse_map;
    extern uint next_id;

    /**
     * @brief Initializes the string and core hash tables with the specified sizes.
     *
     * This function reserves memory for the `str_map` and `core_map` hash tables, optimizing for the 
     * expected number of elements to prevent hash collisions and improve lookup performance.
     *
     * @param str_map_size The initial size of the string hash table (`str_map`). Default is `1000`.
     * @param core_map_size The initial size of the core hash table (`core_map`). Default is `10000`.
     */
    void init_hashing(size_t str_map_size = STR_HASH_TABLE_SIZE, size_t core_map_size = CORE_HASH_TABLE_SIZE);

    /**
     * @brief Computes a hash value for a sequence of bytes.
     *
     * This function hashes a sequence of bytes from the specified range, using a mixing algorithm to 
     * ensure uniform distribution of hash values. It converts all characters to uppercase during the 
     * process to make the hashing case-insensitive.
     *
     * @param begin An iterator to the start of the byte sequence.
     * @param end An iterator to the end of the byte sequence.
     * @param seed An optional seed value for initializing the hash. Default is `0x153ac45c`.
     * @return The computed hash value.
     *
     */
    size_t hash_bytes(std::string::iterator begin, std::string::iterator end, size_t seed=0x153ac45c);

    /**
     * @brief Initializes the reverse mapping from core IDs to core vectors.
     *
     * This function checks if the `next_id` is zero. If it is, the function returns false,
     * indicating that there are no cores to initialize. Otherwise, it resizes the `reverse_map`
     * to match the size of `next_id`, which is the size of the labels, initializing all elements to `nullptr`.
     * Then, it iterates through the `core_map`, setting each entry in `reverse_map` 
     * to point to the corresponding core vector from `core_map` using the core ID as the index.
     *
     * @return true if the reverse map was successfully initialized, false if `next_id` is zero.
     */
    bool init_reverse();

    /**
     * @brief Initializes the core_counts vector to a specified size with all elements set to zero.
     *
     * This function resizes the provided `core_counts` vector to the given `size` and 
     * initializes each element in the vector to zero. It ensures that the vector is 
     * properly sized and all counts are reset.
     *
     * @param core_counts A reference to a vector of unsigned integers representing the core counts.
     * @param size The number of elements the vector should contain.
     * @return true Always returns true after initializing the vector.
     */
    bool init_core_counts( std::vector<uint>& core_counts, size_t size = next_id);

    /**
     * @brief Recursively increments core counts for a given core and its dependencies.
     *
     * This function increments the count for the specified `core` in the `core_counts` vector.
     * If the `reverse_map` for the given core is not null, it recursively increments the 
     * counts for all cores in the associated core vector by calling `count_core` on each 
     * core in the reverse map.
     *
     * @param core_counts A reference to a vector of unsigned integers representing core counts.
     * @param core The index of the core whose count is to be incremented.
     */
    void count_core( std::vector<uint>& core_counts, uint core );

    /**
     * @brief Retrieves labels and counts for sublevel cores and stores them in the provided 
     * sub_labels and sub_count vectors.
     *
     * This function processes cores that are composed of sub cores (sublevel cores). It checks 
     * if the `reverse_map` is empty or if the sizes of `labels` and `core_count` don't match, 
     * in which case it returns false. If the sizes are valid, it reserves space in the `sub_labels` 
     * and `sub_count` vectors based on 3.6 times the size of `labels`. The function iterates 
     * through the `labels` vector and retrieves the corresponding subcores from the `reverse_map`. 
     * For each subcore, it adds its label to `sub_labels` and its count (from `core_count`) to `sub_count`, 
     * provided its count is greater than 0. The subcore's count is then reset to zero.
     *
     * After collecting sublevel labels and counts, the function restores the core counts for the sub_labels.
     *
     * @param labels A reference to a vector of unsigned integers representing the core labels.
     * @param core_count A reference to a vector of unsigned integers representing the count of each core.
     * @param sub_labels A reference to a vector where sublevel core labels will be stored.
     * @param sub_count A reference to a vector where sublevel core counts will be stored.
     * @return true if the sublevel labels and counts were successfully retrieved, false if `reverse_map` 
     *         is empty or if `labels` and `core_count` sizes do not match.
     */
    bool get_sublevel_labels(std::vector<uint>& labels, std::vector<uint>& core_count, std::vector<uint>& sub_labels, std::vector<uint>& sub_count);
};

#endif