/**
 * @file hash.cpp
 * @brief Implements hashing and equality structures along with functions to manage hash tables 
 *        in the LCP parsing system.
 *
 * This source file provides the implementation for custom hash functions and equality operators used 
 * in unordered maps. It also includes functionality to initialize the hash maps (`str_map` and `cores_map`) 
 * and compute hash values for sequences of bytes. The hashing functions are designed to handle 
 * case-insensitive input and work efficiently with large datasets.
 */

#include "hash.h"


namespace lcp {

    // -----------------------------------------------------------------
    // implementation of `MurmurHash3_32`
    // -----------------------------------------------------------------
    inline uint32_t rotl32(uint32_t x, int8_t r) {
        return (x << r) | (x >> (32 - r));
    }

    inline uint32_t fmix32(uint32_t h) {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    }

    inline uint32_t MurmurHash3_32(const void* key, int len, uint32_t seed) {
        const uint8_t* data = (const uint8_t*)key;
        const int nblocks = len / 4;

        uint32_t h1 = seed;

        const uint32_t c1 = 0xcc9e2d51;
        const uint32_t c2 = 0x1b873593;

        // Body: Process blocks of 4 bytes at a time
        const uint32_t* blocks = (const uint32_t*)(data + nblocks * 4);

        for (int i = -nblocks; i; i++) {
            uint32_t k1 = blocks[i];

            k1 *= c1;
            k1 = rotl32(k1, 15);
            k1 *= c2;

            h1 ^= k1;
            h1 = rotl32(h1, 13);
            h1 = h1 * 5 + 0xe6546b64;
        }

        // Tail: Process remaining bytes
        const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);

        uint32_t k1 = 0;

        switch (len & 3) {
            case 3: 
                k1 ^= tail[2] << 16; 
                [[gnu::fallthrough]];
            case 2: 
                k1 ^= tail[1] << 8; 
                [[gnu::fallthrough]];
            case 1: 
                k1 ^= tail[0];
                k1 *= c1; 
                k1 = rotl32(k1, 15); 
                k1 *= c2; h1 ^= k1;
        }

        // Finalization: Mix the hash to ensure the last few bits are fully mixed
        h1 ^= len;
        h1 = fmix32(h1);

        return h1;
    }

    // -----------------------------------------------------------------
    // implementation of `cores struct`
    // -----------------------------------------------------------------
    cores::cores() {};

    cores::cores( const uint32_t core1, const uint32_t core2, const uint32_t core3, const uint32_t middle_count, const uint32_t label ) {
        this->core1 = core1;
        this->core2 = core2;
        this->core3 = core3;
        this->middle_count = middle_count;
        this->label = label;
    };

    bool cores::operator==( const cores& other ) const {
        return core1 == other.core1 && core2 == other.core2 && core3 == other.core3 && middle_count == other.middle_count;
    };

    // -----------------------------------------------------------------
    // implementation of `hash_map`
    // -----------------------------------------------------------------
    hash_map::hash_map( size_t size ) {
        this->table.resize(size);
        this->_size = 0;
        this->_capacity = size;
    };

    void hash_map::reserve( size_t size ) {
        this->_size = std::min(size, this->_size);
        this->_capacity = size;
        this->table.resize(size);
    };

    uint32_t hash_map::emplace( const uint32_t& index, const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count, const uint32_t& label ) {
        bucket_type& row = table[index];
        bucket_type::iterator it = row.begin();
        
        for ( ; it != row.end(); it++ ) {
            if ( it->core1 == core1 && it->core2 == core2 && it->core3 == core3 && it->middle_count == middle_count ) {
                return it->label;
            }
        }
        
        row.insert( it, cores( core1, core2, core3, middle_count, label ) );
        
        this->_size++;
        return label;
    };

    inline std::pair<bool, uint32_t> hash_map::exists( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count ) const {
        const size_t index = entry(core1, core2, core3, middle_count);
        const bucket_type& row = this->table[index];

        // Iterate through the bucket to find the key
        for ( bucket_type::const_iterator it = row.begin(); it != row.end(); it++ ) {
            if ( it->core1 == core1 && it->core2 == core2 && it->core3 == core3 && it->middle_count == middle_count ) {
                return std::make_pair(true, it->label);
            }
        }
        return std::make_pair(false, index);
    };

    inline size_t hash_map::bucket_size( size_t bucket_index ) {
        return this->table[bucket_index].size();
    };

    size_t hash_map::size() {
        return this->_size;
    };

    float hash_map::load_factor() {
        return  static_cast<float>(this->_size) / this->_capacity;
    };

    size_t hash_map::capacity() {
        return this->_capacity;
    };

    // -----------------------------------------------------------------
    // implementation of `hash_map::iterator`
    // -----------------------------------------------------------------
    hash_map::iterator::iterator(TableRowIt rowIt, TableRowIt rowItEnd, BucketIt bucketIt) {
        this->rowIt = rowIt;
        this->rowItEnd = rowItEnd;
        this->bucketIt = bucketIt;
        advance();
    };

    hash_map::iterator& hash_map::iterator::operator++() {
        bucketIt++;
        advance();
        return *(this);
    };

    hash_map::iterator hash_map::iterator::operator++(int) {
        iterator temp = *this;
        ++(*this);
        return temp;
    };
    
    bool hash_map::iterator::operator!=(const hash_map::iterator& other) {
        return this->rowIt != other.rowIt || this->bucketIt != other.bucketIt;
    };

    struct cores& hash_map::iterator::operator*() {
        return *(this->bucketIt);
    };

    struct cores* hash_map::iterator::operator->() {
        return &(*this->bucketIt);
    };

    void hash_map::iterator::advance() {
        while (this->rowIt != this->rowItEnd && this->bucketIt == this->rowIt->end()) {
            this->rowIt++;
            if (this->rowIt != this->rowItEnd) {
                this->bucketIt = this->rowIt->begin();
            }
        }
    };
    
    hash_map::iterator hash_map::begin() {
        return iterator(this->table.begin(), this->table.end(), this->table.begin()->begin());
    };

    hash_map::iterator hash_map::end() {
        return iterator(this->table.end(), this->table.end(), this->table.end()->begin());
    };

    inline uint32_t hash_map::entry( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count ) const {
        uint32_t result = MurmurHash3_32(&(core1), sizeof(core1)); 
        result ^= MurmurHash3_32(&(core2), sizeof(core2)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
        result ^= MurmurHash3_32(&(core3), sizeof(core3)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
        result ^= MurmurHash3_32(&(middle_count), sizeof(middle_count)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
        return result % this->_capacity;
    };

    // -----------------------------------------------------------------
    // implementation of `hashing_cores::operator()`
    // -----------------------------------------------------------------
    std::size_t hashing_cores::operator() ( const struct cores& elements ) const {
        uint32_t result = MurmurHash3_32(&(elements.core1), sizeof(elements.core1)); 
        result ^= MurmurHash3_32(&(elements.core2), sizeof(elements.core2)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
        result ^= MurmurHash3_32(&(elements.core3), sizeof(elements.core3)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
        result ^= MurmurHash3_32(&(elements.middle_count), sizeof(elements.middle_count)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
        return result;
    };

    // -----------------------------------------------------------------
    // implementation of `equality_cores::operator()`
    // -----------------------------------------------------------------
    bool equality_cores::operator() ( const struct cores& lhs, const struct cores& rhs ) const {
        return lhs == rhs;
    };

    // -----------------------------------------------------------------
    // implementation of `hash` functions
    // -----------------------------------------------------------------
    namespace hash {

        // mutex
        std::mutex str_map_mutex;
        std::mutex cores_map_mutex;

        // maps
        std::unordered_map<std::string, uint32_t> str_map;
        cores_map_type cores_map;
        std::vector<const cores_map_key_type*> reverse_map;

        // ID
        uint32_t next_id = 0;
        

        void init(size_t str_map_size, size_t cores_map_size) {
            str_map.reserve(str_map_size);
            cores_map.reserve(cores_map_size);
        };

        uint32_t emplace( std::string::iterator& begin, std::string::iterator end ) {
            std::string kmer = std::string(begin, end);
            std::transform(kmer.begin(), kmer.end(), kmer.begin(), ::toupper);
                            
            if ( str_map.find(kmer) == str_map.end() ) {
                std::lock_guard<std::mutex> lock(str_map_mutex); 
                std::pair<std::unordered_map<std::string, uint32_t>::iterator, bool> result = str_map.emplace(kmer, next_id);
            
                if (result.second) {
                    return next_id++;
                }  else {
                    return result.first->second;
                }
            } else {
                return str_map[kmer];
            }
        };

        uint32_t emplace( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count ) {
            std::pair<bool, uint32_t> results = cores_map.exists( core1, core2, core3, middle_count );

            if ( !(results.first) ) {
                std::lock_guard<std::mutex> lock(cores_map_mutex); 
                uint32_t value = cores_map.emplace( results.second,  core1, core2, core3, middle_count, next_id);
                if ( value == next_id ) {
                    return next_id++;
                }
                return next_id;
            }
            
            return results.second;
        };
        
        uint32_t simple( std::string::iterator& begin, std::string::iterator end ) {
            std::string kmer = std::string(begin, end);
            std::transform(kmer.begin(), kmer.end(), kmer.begin(), ::toupper);
            return MurmurHash3_32(kmer.c_str(), kmer.size());
        }; 
        
        uint32_t simple( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count ) {
            uint32_t result = MurmurHash3_32(&core1, sizeof(core1)); 
            result ^= MurmurHash3_32(&core2, sizeof(core2)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
            result ^= MurmurHash3_32(&core3, sizeof(core3)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
            result ^= MurmurHash3_32(&middle_count, sizeof(middle_count)) + PRIME_MULTIPLIER + (result << 6) + (result >> 2);
            return result;
        };

        void save_maps( std::ofstream& file ) {

            if (!file) {
                throw std::runtime_error("Saving maps failed. Ofstream empty.");
            }
            
            // save string map
            size_t capacity = str_map.max_load_factor() * str_map.bucket_count(), size = str_map.size();
            file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            
            for ( std::unordered_map<std::string, uint32_t>::iterator it = str_map.begin(); it != str_map.end(); it++ ) {
                size_t key_size = it->first.size();
                file.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
                file.write(it->first.c_str(), key_size);
                file.write(reinterpret_cast<const char*>(&(it->second)), sizeof(it->second));
            }

            // save cores map
            capacity = cores_map.capacity();
            size = cores_map.size();

            file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));

            for ( cores_map_type::iterator it = cores_map.begin(); it != cores_map.end(); it++ ) {
                file.write(reinterpret_cast<const char*>(&(*it)), sizeof(*it));
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
            
            str_map.reserve(capacity);

            for ( size_t i = 0; i < map_size; i++ ) {
                size_t key_size;
                file.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
                
                std::string key(key_size, '\0');
                file.read(&key[0], key_size);

                uint32_t value;
                file.read(reinterpret_cast<char*>(&value), sizeof(value));

                str_map[key] = value;
            }
            
            // load cores_map
            file.read(reinterpret_cast<char*>(&capacity), sizeof(capacity));
            file.read(reinterpret_cast<char*>(&map_size), sizeof(map_size));

            cores_map.reserve(capacity);

            for ( size_t i = 0; i < map_size; i++ ) {
                cores_map_key_type key(0, 0, 0, 0, 0);
                file.read(reinterpret_cast<char*>(&key), sizeof(key));

                // TODO - fix core_map initialization
                // uint32_t value;
                // file.read(reinterpret_cast<char*>(&value), sizeof(value));
                
                // cores_map[key] = value;
            }
        };

        bool init_reverse() {
            
            if ( next_id == 0 ) {
                return false;
            }

            reverse_map.resize(next_id, nullptr);
            
            for ( cores_map_type::iterator it = cores_map.begin(); it != cores_map.end(); it++ ) {
                reverse_map[it->label] = &(*it);
            }

            return true;
        };

        void count_core( std::vector<uint32_t>& core_counts, uint32_t core ) {

            core_counts[core]++;
            
            if ( reverse_map[core] == nullptr ) {
                return;
            }
            
            count_core( core_counts, (reverse_map[core])->core1 );
            for ( uint32_t i = 0; i < (reverse_map[core])->middle_count; i++) {
                count_core( core_counts, (reverse_map[core])->core2 );
            }
            count_core( core_counts, (reverse_map[core])->core3 );
        };

        void set_lcp_levels( std::vector<unsigned short>& lcp_levels ) {
            
            size_t size = str_map.size() + cores_map.size();

            lcp_levels.resize(size, 0);

            for ( std::unordered_map<std::string, uint32_t>::iterator it = str_map.begin(); it != str_map.end(); it++ ) {
                lcp_levels[it->second] = 1;
            }

            bool done = false;

            while( !done ) {    
                done = true;        
                for ( cores_map_type::iterator it = cores_map.begin(); it != cores_map.end(); it++ ) {
                    size_t first_subcore_label = 0;
                    first_subcore_label = it->core1;
                    if ( lcp_levels[it->label] != 0 || lcp_levels[first_subcore_label] == 0 ) {
                        continue;
                    }
                    done = false;
                    lcp_levels[it->label] = lcp_levels[first_subcore_label] + 1;
                }
            }
        };

        bool get_sublevel_labels(std::vector<uint32_t>& labels, std::vector<uint32_t>& core_counts, std::vector<uint32_t>& sub_labels, std::vector<uint32_t>& sub_counts) {
            
            if ( reverse_map.size() == 0 ) {
                return false;
            }
            
            sub_labels.reserve( (int)( 3.8 * labels.size() ) );
            sub_counts.reserve( (int)( 3.8 * labels.size() ) );
            
            for ( std::vector<uint32_t>::iterator it_label = labels.begin(); it_label < labels.end(); it_label++ ) {

                const cores_map_key_type* subcores = reverse_map[(*it_label)];

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
            }

            for ( size_t index = 0; index < sub_labels.size(); index++ ) {
                core_counts[sub_labels[index]] = sub_counts[index];
            }

            return true;
        };

        void summary() {
            size_t collisions = 0, empty = 0, max = 0;
            for ( size_t bucket = str_map.bucket_count(); bucket--; ) {
                if ( str_map.bucket_size(bucket) == 0 )
                    empty++;
                else
                    collisions += str_map.bucket_size(bucket) - 1;
                
                max = std::max(max, str_map.bucket_size(bucket));
            }
            std::cout << "str_map = " << str_map.load_factor() << ' ' << str_map.bucket_count() << ' ' << collisions << ' ' << empty << ' ' << max << '\n';

            collisions = 0; empty = 0; max = 0;
            for ( size_t bucket = cores_map.capacity(); bucket--; ) {
                if ( cores_map.bucket_size(bucket) == 0 )
                    empty++;
                else
                    collisions += cores_map.bucket_size(bucket) - 1;
                
                max = std::max(max, cores_map.bucket_size(bucket));
            }

            std::cout << "cores_map = " << cores_map.load_factor() << ' ' << cores_map.capacity() << ' ' << collisions << ' ' << empty << ' ' << max << '\n';
        };

    };
};