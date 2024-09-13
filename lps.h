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
#include "hash.h"

namespace lcp {

    class lps {
    public:
        int level;
        std::vector<core*> *cores;

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
