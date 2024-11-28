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

    namespace hash {

        // mutex
        std::mutex str_map_mutex;
        std::mutex cores_map_mutex;

        // maps
        std::unordered_map<std::string, uint32_t> str_map(0);
        std::vector<std::list<struct cores>> cores_map(0);

        // ID
        uint32_t next_id = 0;
        

        void init( size_t str_map_size, size_t cores_map_size ) {
            str_map.reserve( str_map_size );
            
            if ( cores_map.size() > 0 ) {
                std::cout << "Reserving hash map failed. It is already used." << std::endl;
                return;
            }
            cores_map.reserve( cores_map_size );
        };

        uint32_t emplace( const char* data, size_t length ) {

            (void) length;
            
            // construct the string
            std::string str(data);
            delete[] data;
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);

            // find if it is already exists
            std::unordered_map<std::string, uint32_t>::iterator index = str_map.find(str);

            if ( index != str_map.end() ) {
                return index->second;
            }

            // insert safely
            {
                std::lock_guard<std::mutex> lock(str_map_mutex);
                std::pair<std::unordered_map<std::string, uint32_t>::iterator, bool> result = str_map.emplace(str, next_id);
                
                if ( result.second ) {
                    return next_id++;
                }
                
                return result.first->second;
            }
        };

        uint32_t emplace( const uint32_t* data, size_t length ) {

            size_t index = MurmurHash3_32( data, length * sizeof(uint32_t) ) % cores_map.capacity();
            std::list<struct cores>& bucket = cores_map[index];
            
            for ( const struct cores& core : bucket ) {
                if ( std::equal(std::begin(core.data), std::end(core.data), data)) {
                    delete[] data;
                    return core.label;
                }
            }
            
            {
                std::lock_guard<std::mutex> lock(cores_map_mutex); 
                
                for ( const struct cores& core : bucket ) {
                    if ( std::equal(std::begin(core.data), std::end(core.data), data)) {
                        delete[] data;
                        return core.label;
                    }
                }

                bucket.emplace_back( next_id, data );
                delete[] data;
                return next_id++;
            }
        };
        
        uint32_t simple( const char *data, size_t length ) {
            uint32_t result = MurmurHash3_32( data, length * sizeof(char) );
            delete[] data;
            return result;
        }; 
        
        uint32_t simple( const uint32_t *data, size_t length ) {
            uint32_t result = MurmurHash3_32( data, length * sizeof(uint32_t) );
            delete[] data;
            return result;
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
            for ( size_t bucket = cores_map.size(); bucket--; ) {
                if ( cores_map[bucket].size() == 0 )
                    empty++;
                else
                    collisions += cores_map[bucket].size() - 1;
                
                max = std::max( max, cores_map[bucket].size() );
            }

            std::cout << "cores_map = " << static_cast<float>(cores_map.size()) / cores_map.capacity() << ' ' << cores_map.capacity() << ' ' << collisions << ' ' << empty << ' ' << max << '\n';
        };

        // Hash functions

        inline uint64_t MurmurHash64A( const void * key, int len, uint64_t seed ) {
            const uint64_t m = BIG_CONSTANT(0xc6a4a7935bd1e995);
            const int r = 47;

            uint64_t h = seed ^ (len * m);

            const uint64_t * data = (const uint64_t *)key;
            const uint64_t * end = data + (len/8);

            while(data != end) {
                uint64_t k = *data++;

                k *= m; 
                k ^= k >> r; 
                k *= m; 
        
                h ^= k;
                h *= m; 
            }

            const unsigned char * data2 = (const unsigned char*)data;

            switch(len & 7) {
            case 7: h ^= uint64_t(data2[6]) << 48; [[gnu::fallthrough]];
            case 6: h ^= uint64_t(data2[5]) << 40; [[gnu::fallthrough]];
            case 5: h ^= uint64_t(data2[4]) << 32; [[gnu::fallthrough]];
            case 4: h ^= uint64_t(data2[3]) << 24; [[gnu::fallthrough]];
            case 3: h ^= uint64_t(data2[2]) << 16; [[gnu::fallthrough]];
            case 2: h ^= uint64_t(data2[1]) << 8;  [[gnu::fallthrough]];
            case 1: h ^= uint64_t(data2[0]);
                    h *= m;
            };

            h ^= h >> r;
            h *= m;
            h ^= h >> r;

            return h;
        };

        inline uint32_t rotl32( uint32_t x, int8_t r ) {
            return (x << r) | (x >> (32 - r));
        };

        inline uint32_t fmix32( uint32_t h ) {
            h ^= h >> 16;
            h *= 0x85ebca6b;
            h ^= h >> 13;
            h *= 0xc2b2ae35;
            h ^= h >> 16;
            return h;
        };

        inline uint32_t MurmurHash3_32( const void* key, int len, uint32_t seed ) {
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
    };
};