#ifndef CONSTANT_H
#define CONSTANT_H

#include <cstdint>

typedef uint32_t ublock;
typedef uint32_t ulabel;
typedef uint32_t ubit_size;

#define ASCII_SIZE              128
#define LCP_VERBOSE             false
#define LCP_USE_MAP             false
#define LCP_REV_COMP            false
#define LCP_SHRINK_VECTOR       false
#define UBLOCK_BIT_SIZE         32
#define DCT_ITERATION_COUNT     1
#define CONSTANT_FACTOR         1.5
#define DICT_BIT_SIZE           2
#define STR_HASH_TABLE_SIZE     1000
#define CORE_HASH_TABLE_SIZE    10000
#define MAX_STR_LENGTH          1000000
#define OVERLAP_MARGIN          10000
#define MEMCOMP_CORES_SIZE      4 * sizeof(ublock)

#endif
