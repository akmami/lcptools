#ifndef STRING_H
#define STRING_H

typedef unsigned char uchar;

#define SIZE_PER_BLOCK                  8
#define COMPRESSION_ITERATION_COUNT     1
#define MAXIMUM_LENGTH                  10000
#define CONSTANT_FACTOR                 2

#include <iostream>
#include <string>
#include <vector>
#include "encoding.h"
#include "base_core.h"
#include "core.h"

namespace lcp {

    class string {
    public:
        size_t start_index;     // For each DCT, first core is not compressed, hence needs to be removed.
                                // However, removing first element in vector will cause all the rest to be shifted
                                // left, which is 0(N) time task. In order to prevent such inefficient call,
                                // start index for vector can be shifted to right. This variable serves to that purpose.
        std::vector<core*> cores;
        std::vector<base_core*> base_cores;
        int level;

        string(std::string &str, bool rev_comp = false);
        ~string();

        template<typename Iterator>
        void process_string(Iterator it1, Iterator it2, Iterator end, bool rev_comp = false);
        bool deepen();
        bool deepen(int lcp_level);
    };

    std::ostream& operator<<(std::ostream& os, const string& element);
    std::ostream& operator<<(std::ostream& os, const string* element);
};

#endif
