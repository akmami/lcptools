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
    core_map_type core_map;
    std::vector<const core_map_key_type*> reverse_map;
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

    bool init_reverse() {
        
        if ( next_id == 0 ) {
            return false;
        }

        reverse_map.resize(next_id, nullptr);
        
        for ( core_map_type::iterator it = core_map.begin(); it != core_map.end(); it++ ) {
            reverse_map[it->second] = &(it->first);
        }

        return true;
    };

    bool init_core_counts( std::vector<uint>& core_counts, size_t size ) {
        
        core_counts.resize(size);

        for( size_t i = 0; i < size; i++ ) {
            core_counts[i] = 0;
        }

        return true;
    };

    void count_core( std::vector<uint>& core_counts, uint core ) {

        core_counts[core]++;
        
        if ( reverse_map[core] == nullptr ) {
            return;
        }

        #ifdef MAP_KEY_VECTOR
        for ( core_map_key_type::const_iterator it = (*(reverse_map[core])).begin(); it != (*(reverse_map[core])).end(); it++ ) {
            count_core( core_counts, *it );
        }
        #else
        count_core( core_counts, (reverse_map[core])->core1 );
        for ( uint i = 0; i < (reverse_map[core])->middle_count; i++) {
            count_core( core_counts, (reverse_map[core])->core2 );
        }
        count_core( core_counts, (reverse_map[core])->core3 );
        #endif
    };

    bool get_sublevel_labels(std::vector<uint>& labels, std::vector<uint>& core_counts, std::vector<uint>& sub_labels, std::vector<uint>& sub_counts) {
        
        if ( reverse_map.size() == 0 ) {
            return false;
        }
        
        sub_labels.reserve( (int)( 3.8 * labels.size() ) );
        sub_counts.reserve( (int)( 3.8 * labels.size() ) );
        
        for ( std::vector<uint>::iterator it_label = labels.begin(); it_label < labels.end(); it_label++ ) {

            const core_map_key_type* subcores = reverse_map[(*it_label)];

            #ifdef MAP_KEY_VECTOR
            for ( core_map_key_type::const_iterator it_sublabel = subcores->begin(); it_sublabel != subcores->end(); it_sublabel++ ) {
                if ( core_counts[*it_sublabel] > 0 ) {
                    sub_labels.push_back(*it_sublabel);
                    sub_counts.push_back(core_counts[(*it_sublabel)]);

                    core_counts[(*it_sublabel)] = 0;
                }
            }
            #else
            if ( core_counts[subcores->core1] > 0 ) {
                sub_labels.push_back( subcores->core1 );
                sub_counts.push_back( core_counts[subcores->core1] );
                core_counts[subcores->core1] = 0;
            }
            if ( core_counts[subcores->core2] > 0 ) {
                sub_labels.push_back( subcores->core2 );
                sub_counts.push_back( core_counts[subcores->core2] );
                core_counts[subcores->core2] = 0;
            }
            if ( core_counts[subcores->core3] > 0 ) {
                sub_labels.push_back( subcores->core3 );
                sub_counts.push_back( core_counts[subcores->core3] );
                core_counts[subcores->core3] = 0;
            }
            #endif
        }

        for ( size_t index = 0; index < sub_labels.size(); index++ ) {
            core_counts[sub_labels[index]] = sub_counts[index];
        }

        return true;
    };
};