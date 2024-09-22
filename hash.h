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
#include <mutex>
#include <string>
#include <vector>
#include <iterator>
#include <cstddef>
#include <cstring>
#include <fstream>
#include "lps.h"
#include "constant.h"


#define M       0x5bd1e995;
#define SEED    0x153ac45f;
// #define MAP_KEY_VECTOR


struct cores {
    uint core1;
    uint core2;
    uint core3;
    uint middle_count;
    
    bool operator==(const cores& other) const {
        return core1 == other.core1 && core2 == other.core2 && core3 == other.core3 && middle_count == other.middle_count;
    }
};


struct hashing_cores {
    std::size_t operator() ( const struct cores& elements ) const {
        std::size_t hash = SEED;
        uint element;

        element = elements.core1;
        element *= M;
        element ^= element >> 24;
        element *= M;
        hash *= M;
        hash ^= element;

        element = elements.core2;
        element *= M;
        element ^= element >> 24;
        element *= M;
        hash *= M;
        hash ^= element;

        element = elements.core3;
        element *= M;
        element ^= element >> 24;
        element *= M;
        element *= M;
        hash ^= element;

        // do a few final mixes of the hash.
        hash ^= hash >> 13;
        hash *= M;
        hash ^= hash >> 15;
        return hash;
    };
};


struct equality_cores {
    bool operator() ( const struct cores& lhs, const struct cores& rhs ) const {
        return lhs == rhs;
    };
};


struct hashing_vector {
    std::size_t operator() ( const std::vector<uint>& vec ) const {
        std::size_t hash = SEED;
        uint element;

        for(size_t i = 0; i < vec.size(); i++ ) {
            // hash element
            element = vec[i];
            element *= M;
            element ^= element >> 24;
            element *= M;
            hash *= M;
            hash ^= element;
        }

        // do a few final mixes of the hash.
        hash ^= hash >> 13;
        hash *= M;
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

    extern std::mutex str_map_mutex;
    extern std::mutex core_map_mutex;
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
    void init_hashing( size_t str_map_size = STR_HASH_TABLE_SIZE, size_t core_map_size = CORE_HASH_TABLE_SIZE );

    /**
     * @brief Saves the contents of `str_map` and `core_map` to a binary file.
     * 
     * This function serializes two maps (`str_map` and `core_map`) and writes them to the provided 
     * output file stream. The first map is an `unordered_map` with `std::string` keys and `uint` values, 
     * and the second map has `cores` (a custom struct) as keys and `uint` values. For each map, the 
     * function writes the capacity, size, and then the key-value pairs.
     *
     * @param file The output file stream (`std::ofstream`) where the map data will be written. 
     *        The file must already be open.
     * 
     * @throws std::runtime_error If the provided file stream is not open.
     */
    void save_maps( std::ofstream& file );

    /**
     * @brief Loads the contents of `str_map` and `core_map` from a binary file.
     * 
     * This function deserializes two maps (`str_map` and `core_map`) from the provided input file 
     * stream. It reads the capacity and size of each map, reserves the necessary space, and then 
     * reconstructs the map by reading key-value pairs from the file.
     *
     * @param file The input file stream (`std::ifstream`) from which the map data will be read. 
     *        The file must already be open and contain the serialized data from a previous `save()` call.
     * 
     * @throws std::runtime_error If the provided file stream is not open.
     */
    void load_maps( std::ifstream& file );

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
     * @brief Sets the LCP levels for each core in the dataset.
     * 
     * This function assigns LCP levels to each core based on the structure of the core
     * and its subcores. The cores can either be constructed from strings or from a combination
     * of subcores.
     * 
     * @param lcp_levels A reference to a vector of unsigned integers representing the LCP levels.
     *                   The function modifies this vector by setting LCP levels for each core.
     */
    void set_lcp_levels( std::vector<unsigned short>& lcp_levels );

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
    bool get_sublevel_labels( std::vector<uint>& labels, std::vector<uint>& core_count, std::vector<uint>& sub_labels, std::vector<uint>& sub_count );
};

#endif