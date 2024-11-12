#ifndef   CONSTANT_H
#define   CONSTANT_H

typedef unsigned int ublock;

#define ASCII_SIZE                      128
#define LCP_VERBOSE                     false
#define LCP_USE_MAP                     false
#define LCP_REV_COMP                    false
#define LCP_SHRINK_VECTOR               false
#define SIZE_PER_BLOCK                  32
#define COMPRESSION_ITERATION_COUNT     1
#define CONSTANT_FACTOR                 2
#define DICT_BIT_SIZE                   2
#define STR_HASH_TABLE_SIZE             1000
#define CORE_HASH_TABLE_SIZE            10000
#define MAX_STR_LENGTH                  1000000
#define OVERLAP_MARGIN                  10000

#endif
