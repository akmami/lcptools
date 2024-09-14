/**
 * @file hash.cpp
 * @brief Implements hashing and equality structures along with functions to manage hash tables 
 *        in the LCP parsing system.
 *
 * This source file provides the implementation for custom hash functions and equality operators used 
 * in unordered maps. It also includes functionality to initialize the hash maps (`str_map` and `core_map`) 
 * and compute hash values for sequences of bytes. The hashing functions are designed to handle 
 * case-insensitive input and work efficiently with large datasets.
 */

#include "hash.h"


namespace lcp {

    std::unordered_map<std::string, uint> str_map;
    std::unordered_map<std::array<uint, 3>, uint, hashing, equality> core_map;
    uint next_id = 0;
    

    void init_hashing(size_t str_map_size, size_t core_map_size) {
        lcp::str_map.reserve(str_map_size);
        lcp::core_map.reserve(core_map_size);
    };


    size_t hash_bytes(std::string::iterator begin, std::string::iterator end, size_t seed) {
        const size_t m = 0x5bd1e995;
        size_t len = std::distance(begin, end);
        size_t hash = seed ^ len;
        const char* buf = &(*begin);

        // mix 4 bytes at a time into the hash.
        while (len >= 4) {
            size_t chars;
            std::memcpy(&chars, buf, sizeof(chars));
            chars |= 0x20202020; // convert all characters to uppercase. my impl idea :)
            chars *= m;
            chars ^= chars >> 24;
            chars *= m;
            hash *= m;
            hash ^= chars;
            buf += 4;
            len -= 4;
        }

        // handle the last few bytes of the input array.
        switch (len) {
            case 3:
                hash ^= ( static_cast<unsigned char>(buf[2]) | 0x20 ) << 16;
                [[gnu::fallthrough]];
            case 2:
                hash ^= ( static_cast<unsigned char>(buf[1]) | 0x20 ) << 8;
                [[gnu::fallthrough]];
            case 1:
                hash ^= ( static_cast<unsigned char>(buf[0]) | 0x20 );
                hash *= m;
        };

        // do a few final mixes of the hash.
        hash ^= hash >> 13;
        hash *= m;
        hash ^= hash >> 15;
        return hash;
    };
};