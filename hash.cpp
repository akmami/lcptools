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

    std::mutex str_map_mutex;
    std::mutex core_map_mutex;
    std::unordered_map<std::string, uint> str_map;
    core_map_type core_map;
    std::vector<const core_map_key_type*> reverse_map;
    uint next_id = 0;
    

    void init_hashing(size_t str_map_size, size_t core_map_size) {
        lcp::str_map.reserve(str_map_size);
        lcp::core_map.reserve(core_map_size);
    };

    void save_maps( std::ofstream& file ) {

        if (!file) {
            throw std::runtime_error("Saving maps failed. Ofstream empty.");
        }
        
        size_t capacity = str_map.max_load_factor() * str_map.bucket_count(), size = str_map.size();
        file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        
        for ( std::unordered_map<std::string, uint>::iterator it = str_map.begin(); it != str_map.end(); it++ ) {
            size_t key_size = it->first.size();
            file.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
            file.write(it->first.c_str(), key_size);
            file.write(reinterpret_cast<const char*>(&(it->second)), sizeof(it->second));
        }

        capacity = core_map.max_load_factor() * core_map.bucket_count();
        size = core_map.size();

        file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));

        for ( core_map_type::iterator it = core_map.begin(); it != core_map.end(); it++ ) {
            file.write(reinterpret_cast<const char*>(&(it->first)), sizeof(it->first));
            file.write(reinterpret_cast<const char*>(&(it->second)), sizeof(it->second));
        }
    };

    void load_maps( std::ifstream& file ) {

        if (!file) {
            throw std::runtime_error("Loading maps failed. Ifstream empty.");
        }

        size_t capacity;
        size_t map_size;
        
        // load str_map
        file.read(reinterpret_cast<char*>(&capacity), sizeof(capacity));
        file.read(reinterpret_cast<char*>(&map_size), sizeof(map_size));
        
        lcp::str_map.reserve(capacity);

        for ( size_t i = 0; i < map_size; i++ ) {
            size_t key_size;
            file.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
            
            std::string key(key_size, '\0');
            file.read(&key[0], key_size);

            uint value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));

            str_map[key] = value;
        }
        
        // load core_map
        file.read(reinterpret_cast<char*>(&capacity), sizeof(capacity));
        file.read(reinterpret_cast<char*>(&map_size), sizeof(map_size));

        lcp::core_map.reserve(capacity);

        for ( size_t i = 0; i < map_size; i++ ) {
            core_map_key_type key;
            file.read(reinterpret_cast<char*>(&key), sizeof(key));

            uint value;
            file.read(reinterpret_cast<char*>(&value), sizeof(value));
            
            core_map[key] = value;
        }
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

    void set_lcp_levels( std::vector<unsigned short>& lcp_levels ) {
        
        size_t size = str_map.size() + core_map.size();

        lcp_levels.resize(size, 0);

        for ( std::unordered_map<std::string, uint>::iterator it = str_map.begin(); it != str_map.end(); it++ ) {
            lcp_levels[it->second] = 1;
        }

        bool done = false;

        while( !done ) {    
            done = true;        
            for ( core_map_type::iterator it = core_map.begin(); it != core_map.end(); it++ ) {
                size_t first_subcore_label = 0;
                #ifdef MAP_KEY_VECTOR
                first_subcore_label = it->first[0];
                #else
                first_subcore_label = it->first.core1;
                #endif
                if ( lcp_levels[it->second] != 0 || lcp_levels[first_subcore_label] == 0 ) {
                    continue;
                }
                done = false;
                lcp_levels[it->second] = lcp_levels[first_subcore_label] + 1;
            }
        }
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