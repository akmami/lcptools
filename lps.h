#ifndef LPS_H
#define LPS_H

#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>
#include "constant.h"
#include "encoding.h"
#include "core.h"

namespace lcp {

    class lps {
    public:
        int level;
        size_t start_index;     // For each DCT, first core is not compressed, hence needs to be removed.
                                // However, removing first element in vector will cause all the rest to be shifted
                                // left, which is 0(N) time task. In order to prevent such inefficient call,
                                // start index for vector can be shifted to right. This variable serves to that purpose.
        std::vector<core*> cores;
        

        lps(std::string &str, bool rev_comp = false);
        lps(std::ifstream& in);
        ~lps();

        void parse(std::string::iterator begin, std::string::iterator end, bool rev_comp = false);
        bool deepen();
        bool deepen(int lcp_level);
        void write(std::string filename) const;
        void write(std::ofstream& out) const;
        double memsize();
    };

    std::ostream& operator<<(std::ostream& os, const lps& element);
    std::ostream& operator<<(std::ostream& os, const lps* element);
};

#endif
