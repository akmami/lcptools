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
 * ----------------------------------------------------------------------------
 * MurmurHash3 was written by Austin Appleby, and is placed in the public domain.
 * The author hereby disclaims copyright to this source code.
 *
 * Note: This is a 32-bit version of MurmurHash3, adapted from the original
 * implementation. The original MurmurHash3 includes x86 and x64 versions
 * optimized for their respective platforms, which do not produce the same
 * results.
 * 
 * You can find the original MurmurHash3 code here:
 * https://github.com/aappleby/smhasher
 * ----------------------------------------------------------------------------
 * 
 * This version has been adapted for use as a consistent hashing function with 
 * a fixed seed to ensure reproducibility across different runs and platforms.
 * ----------------------------------------------------------------------------
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

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <list>
#include "lps.h"
#include "constant.h"


#define PRIME_MULTIPLIER    0x9e3779b9


namespace lcp {

    inline uint32_t fmix32(uint32_t h);

    inline uint32_t rotl32(uint32_t x, int8_t r);

    uint32_t MurmurHash3_32(const void* key, int len, uint32_t seed = 42);

    struct cores {
        uint32_t core1;
        uint32_t core2;
        uint32_t core3;
        uint32_t middle_count;
        uint32_t label;

        /**
         * @brief Default constructor for the `cores` structure.
         * Initializes the structure with default values.
         */
        cores();

        /**
         * @brief Parameterized constructor for the `cores` structure.
         * 
         * @param core1 First core value.
         * @param core2 Second core value.
         * @param core3 Third core value.
         * @param middle_count Middle count value.
         * @param label Label associated with the structure.
         * 
         * Initializes the structure with the provided core values, middle count, and label.
         */
        cores( const uint32_t core1, const uint32_t core2, const uint32_t core3, const uint32_t middle_count, const uint32_t label );

        /**
         * @brief Equality operator for comparing two `cores` structures.
         * 
         * @param other The other `cores` structure to compare against.
         * 
         * @return true if the two structures are equal based on core values and middle count, false otherwise.
         */
        bool operator==( const cores& other ) const;
    };


    using bucket_type = typename std::list<struct cores>;
    using hash_map_type = typename std::vector<bucket_type>;
    using TableRowIt = typename hash_map_type::iterator;
    using BucketIt = typename bucket_type::iterator;


    class hash_map {
    public:
        /**
         * @brief Constructor for `hash_map` class.
         * 
         * @param size Initial size of the hash table (default is 1009).
         * 
         * Initializes the hash map with a specified size.
         */
        hash_map( size_t size = 1009 );

        /**
         * @brief Reserves space for the hash map by resizing the table.
         * 
         * @param size The new size for the hash table.
         * 
         * This function resizes the hash map and adjusts its size and capacity.
         */
        void reserve( size_t size );

        /**
         * @brief Inserts a new `cores` structure into the hash map if it does not already exist.
         * 
         * This function checks if a `cores` structure with the same values already exists in the bucket.
         * If it does, it returns the label of the existing cores. Otherwise, it inserts the new `cores` structure and returns its label.
         * 
         * @param index Hash index (bucket index) where the cores should be inserted.
         * @param core1 First core value.
         * @param core2 Second core value.
         * @param core3 Third core value.
         * @param middle_count Middle count value.
         * @param label Label for the cores.
         * 
         * @return The label of the inserted or existing cores.
         */
        uint32_t emplace( const uint32_t& index, const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count, const uint32_t& label );

        /**
         * @brief Checks if a specific `cores` structure exists in the hash map.
         * 
         * @param core1 First core value.
         * @param core2 Second core value.
         * @param core3 Third core value.
         * @param middle_count Middle count value.
         * 
         * @return A pair where the first value indicates if the cores exists, and the second value is the label itself or bucket index.
         */
        inline std::pair<bool, uint32_t> exists( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count ) const;

        /**
         * @brief Returns the size of a bucket in the hash table.
         * 
         * @param bucket_index Index of the bucket.
         * 
         * @return The size of the bucket (number of elements in it).
         */
        inline size_t bucket_size( size_t bucket_index );

        /**
         * @brief Returns the number of elements in the hash map.
         * 
         * @return The number of elements in the hash map.
         */
        size_t size();

        /**
         * @brief Calculates the load factor of the hash map.
         * 
         * @return The load factor, which is the ratio of the number of elements to the capacity.
         */
        float load_factor();

        /**
         * @brief Returns the capacity of the hash map (the total number of buckets).
         * 
         * @return The capacity of the hash map.
         */
        size_t capacity();

        // hash_map::iterator class definition
        class iterator {
        public:

            /**
             * @brief Constructor for the hash map iterator.
             * 
             * @param rowIt Iterator pointing to the current row in the hash map.
             * @param rowItEnd Iterator pointing to the end of the rows in the hash map.
             * @param bucketIt Iterator pointing to the current bucket in the hash map.
             */
            iterator(TableRowIt rowIt, TableRowIt rowItEnd, BucketIt bucketIt);

            /**
             * @brief Pre-increment operator for advancing the iterator.
             * 
             * @return Reference to the incremented iterator.
             */
            iterator& operator++();

            /**
             * @brief Post-increment operator for advancing the iterator.
             * 
             * @return A copy of the iterator before it was incremented.
             */
            iterator operator++(int);

            /**
             * @brief Inequality operator for comparing two iterators.
             * 
             * @param other The other iterator to compare against.
             * 
             * @return true if the iterators are not equal, false otherwise.
             */
            bool operator!=(const iterator& other);

            /**
             * @brief Dereference operator for accessing the value pointed to by the iterator.
             * 
             * @return Reference to the `cores` structure at the iterator's position.
             */
            struct cores& operator*();

            /**
             * @brief Arrow operator for accessing members of the `cores` structure at the iterator's position.
             * 
             * @return Pointer to the `cores` structure at the iterator's position.
             */
            struct cores* operator->();

        private:
            /**
             * @brief Advances the iterator to the next valid position.
             * Skips over empty buckets in the hash map.
             */
            void advance();

            TableRowIt rowIt, rowItEnd;
            BucketIt bucketIt;
        };

        /**
         * @brief Returns an iterator to the beginning of the hash map.
         * 
         * @return An iterator pointing to the first element in the hash map.
         */
        iterator begin();
        
        /**
         * @brief Returns an iterator to the end of the hash map.
         * 
         * @return An iterator pointing past the last element in the hash map.
         */
        iterator end();

    private:
        
        size_t _size;
        size_t _capacity;
        hash_map_type table;

        /**
         * @brief Hash function for computing the index in the hash table based on the cores values.
         * 
         * @param core1 First core value.
         * @param core2 Second core value.
         * @param core3 Third core value.
         * 
         * @return The computed hash index for the cores.
         */
        inline uint32_t entry( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count ) const;
    };

    /**
     * @brief Functor for computing the hash of a `cores` structure.
     * 
     * This structure defines a hash function for the `cores` type, allowing it 
     * to be used in hash-based containers like `unordered_map` or `unordered_set`.
     */
    struct hashing_cores {
        /**
         * @brief Computes the hash of a given `cores` structure.
         * 
         * This operator overload allows for efficient hashing of `cores` objects 
         * based on their core values and middle count.
         * 
         * @param elements The `cores` structure for which to compute the hash.
         * 
         * @return The computed hash value as a size_t.
         */
        std::size_t operator() ( const struct cores& elements ) const;
    };

    /**
     * @brief Functor for checking equality between two `cores` structures.
     * 
     * This structure defines an equality comparison for the `cores` type, 
     * enabling its use in containers that require key equality checks.
     */
    struct equality_cores {
        /**
         * @brief Checks if two `cores` structures are equal.
         * 
         * @param lhs The left-hand side `cores` structure.
         * @param rhs The right-hand side `cores` structure.
         * 
         * @return true if the two `cores` structures are equal, false otherwise.
         * 
         * This operator overload allows for straightforward comparison of 
         * `cores` objects based on their core values and middle count.
         */
        bool operator() ( const struct cores& lhs, const struct cores& rhs ) const;
    };


    using cores_map_key_type = struct cores;
    using cores_map_type = hash_map;


    namespace hash {

        extern std::mutex str_map_mutex;
        extern std::mutex cores_map_mutex;
        extern std::unordered_map<std::string, uint32_t> str_map;
        extern cores_map_type cores_map;
        extern std::vector<const cores_map_key_type*> reverse_map;
        extern uint32_t next_id;

        /**
         * @brief Initializes the string and core hash tables with the specified sizes.
         *
         * This function reserves memory for the `str_map` and `core_map` hash tables, optimizing for the 
         * expected number of elements to prevent hash collisions and improve lookup performance.
         *
         * @param str_map_size The initial size of the string hash table (`str_map`). Default is `1000`.
         * @param core_map_size The initial size of the core hash table (`core_map`). Default is `10000`.
         */
        void init( size_t str_map_size = STR_HASH_TABLE_SIZE, size_t core_map_size = CORE_HASH_TABLE_SIZE );

        /**
         * @brief Inserts a k-mer string into str_map and returns its associated unique identifier.
         * 
         * This function extracts a substring (k-mer) from the provided range of iterators (begin to end), converts it to uppercase,
         * and attempts to insert it into the str_map hash map. If the k-mer is not already present in the map, a new unique identifier is assigned
         * to it (next_id), which is then returned. If the k-mer already exists, the existing identifier is returned.
         * The function ensures thread safety by using a std::lock_guard to protect access to the str_map during insertion.
         * 
         * @param begin An iterator pointing to the beginning of the k-mer substring within the input string.
         * @param end An iterator pointing to the end of the k-mer substring within the input string.
         * @return The unique identifier (uint32_t) associated with the k-mer. If the k-mer is newly inserted, next_id is incremented.
        */
        uint32_t emplace( std::string::iterator& begin, std::string::iterator end );

        /**
         * @brief Inserts a set of core values into cores_map and returns its associated unique identifier.
         * 
         * This function checks if a combination of four uint32_t values (core1, core2, core3, and middle_count) exists in the cores_map.
         * If the combination is not already present, the function inserts the new core values into the cores_map with a unique identifier (next_id).
         * If the core values already exist, their associated identifier is returned.
         * The function ensures thread safety by using a std::lock_guard to protect access to the cores_map during insertion.
         * 
         * @param core1 First unsigned integer value representing a core component.
         * @param core2 Second unsigned integer value representing a core component.
         * @param core3 Third unsigned integer value representing a core component.
         * @param middle_count Fourth unsigned integer value representing the middle count.
         * @return The unique identifier (uint32_t) associated with the core values. If the core values are newly inserted, next_id is incremented.
        */
        uint32_t emplace( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count );
        
        /**
         * @brief Hashes a k-mer string using MurmurHash3.
         *
         * This function takes a range of characters from a string (specified by 
         * iterators), constructs a k-mer from that range, converts it to uppercase, 
         * and computes its hash using the MurmurHash3_32 function.
         *
         * @param begin An iterator pointing to the beginning of the k-mer string.
         * @param end An iterator pointing to one past the end of the k-mer string.
         *
         * @return The 32-bit hash value of the k-mer string.
         *
         * @note The input range defined by [begin, end) should contain valid characters 
         * that can be transformed to uppercase. The resulting hash value will be 
         * consistent for the same input string across different executions, provided the 
         * same MurmurHash3 implementation is used.
         */
        uint32_t simple( std::string::iterator& begin, std::string::iterator end );

        /**
         * @brief Computes a hash value for four unsigned integer inputs.
         *
         * This function takes four `uint32_t32_t` values (`core1`, `core2`, `core3`, `middle_count`)
         * and combines their individual hash values using XOR and a prime number multiplier (0x9e3779b9).
         * The combination ensures a reasonable distribution of hash values, which is suitable
         * for use in hash-based containers such as `std::unordered_map` or avoid them.
         *
         * @param core1 First unsigned integer value to be hashed.
         * @param core2 Second unsigned integer value to be hashed.
         * @param core3 Third unsigned integer value to be hashed.
         * @param middle_count Fourth unsigned integer value to be hashed.
         * 
         * @return A combined `size_t` hash value computed from the input values.
         */
        uint32_t simple( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count );

        /**
         * @brief Saves the contents of `str_map` and `core_map` to a binary file.
         * 
         * This function serializes two maps (`str_map` and `core_map`) and writes them to the provided 
         * output file stream. The first map is an `unordered_map` with `std::string` keys and `uint32_t` values, 
         * and the second map has `cores` (a custom struct) as keys and `uint32_t` values. For each map, the 
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
        void count_core( std::vector<uint32_t>& core_counts, uint32_t core );

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
        bool get_sublevel_labels( std::vector<uint32_t>& labels, std::vector<uint32_t>& core_count, std::vector<uint32_t>& sub_labels, std::vector<uint32_t>& sub_count );

        /**
         * @brief Provides a summary of hash map statistics for two maps: `str_map` and `cores_map`.
         *
         * This function computes and outputs various statistics about two hash maps (`str_map` and `cores_map`),
         * including the load factor, bucket count, number of collisions, number of empty buckets, and the maximum bucket size.
         * The summary provides insight into the efficiency of the hash maps and their underlying bucket distribution.
         * 
         * The statistics are printed in the following format for each map:
         * 
         *     map_name = <load_factor> <bucket_count> <collisions> <empty_buckets> <max_bucket_size>
         * 
         * Where:
         * - `load_factor` is the ratio of the number of elements to the number of buckets.
         * - `bucket_count` is the total number of buckets.
         * - `collisions` refers to the number of hash collisions (entries sharing the same bucket).
         * - `empty_buckets` is the number of buckets with no entries.
         * - `max_bucket_size` is the largest number of entries in any single bucket.
         */
        void summary();
    };
};

#endif