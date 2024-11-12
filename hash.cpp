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
    };

    inline uint32_t fmix32(uint32_t h) {
        h ^= h >> 16;
        h *= 0x85ebca6b;
        h ^= h >> 13;
        h *= 0xc2b2ae35;
        h ^= h >> 16;
        return h;
    };
    
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
    };

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
        uint32_t data[] = { core1, core2, core3, middle_count };
        return MurmurHash3_32(data, sizeof(data)) % this->_capacity;
    };

    namespace hash {

        // mutex
        std::mutex str_map_mutex;
        std::mutex cores_map_mutex;

        // maps
        std::unordered_map<std::string, uint32_t> str_map;
        hash_map cores_map;

        // ID
        uint32_t next_id = 0;
        

        void init(size_t str_map_size, size_t cores_map_size) {
            str_map.reserve(str_map_size);
            cores_map.reserve(cores_map_size);
        };

        uint32_t emplace( std::string::iterator begin, std::string::iterator end ) {
            
            std::string kmer = std::string(begin, end);
            std::transform(kmer.begin(), kmer.end(), kmer.begin(), ::toupper);
            std::unordered_map<std::string, uint32_t>::iterator index = str_map.find(kmer);

            if ( index != str_map.end() ) {
                return index->second;
            }

            std::lock_guard<std::mutex> lock(str_map_mutex); 
            std::pair<std::unordered_map<std::string, uint32_t>::iterator, bool> result = str_map.emplace(kmer, next_id);
            
            if (result.second) {
                return next_id++;
            }

            return result.first->second;
        };

        uint32_t emplace( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t& middle_count ) {
            
            std::pair<bool, uint32_t> results = cores_map.exists( core1, core2, core3, middle_count );

            if ( results.first ) {
                return results.second;
            }

            std::lock_guard<std::mutex> lock(cores_map_mutex); 
            uint32_t value = cores_map.emplace( results.second, core1, core2, core3, middle_count, next_id );
            
            if ( value == next_id ) {
                return next_id++;
            }

            return next_id;
        };
        
        uint32_t simple( std::string::iterator begin, std::string::iterator end ) {
            
            std::string kmer = std::string(begin, end);
            std::transform(kmer.begin(), kmer.end(), kmer.begin(), ::toupper);
            
            return MurmurHash3_32(kmer.c_str(), kmer.size());
        }; 
        
        uint32_t simple( const uint32_t& core1, const uint32_t& core2, const uint32_t& core3, const uint32_t middle_count ) {
            
            uint32_t data[] = { core1, core2, core3, middle_count };
            
            return MurmurHash3_32(data, sizeof(data));
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