#ifndef   CORE_H
#define   CORE_H

#include <vector>
#include <fstream>
#include <iterator>
#include "constant.h"
#include "encoding.h"

namespace lcp {
	class core {
    public:
        // Core related variables
		size_t start;
		size_t end;

		// Represenation related variables
		size_t block_number;
		size_t start_index;
		ublock* p;

		core(std::string::iterator it1, std::string::iterator it2, size_t start_index, size_t end_index, bool rev_comp = false);
		core(std::vector<core*>::iterator it1, std::vector<core*>::iterator it2);
		core(ublock* p, size_t block_number, size_t start_index, size_t start, size_t end);
		core(std::ifstream& in);
		~core();

        bool get(int index) const;
		void compress(const core* other);
		uint label();
		void write(std::ofstream& out);
	};

    // core operator overloads
	bool operator == (const core& lhs, const core& rhs);
	bool operator > (const core& lhs, const core& rhs);
	bool operator < (const core& lhs, const core& rhs);
	bool operator != (const core& lhs, const core& rhs);
	bool operator >= (const core& lhs, const core& rhs);
	bool operator <= (const core& lhs, const core& rhs);

	std::ostream& operator<<(std::ostream& os, const core& element);
	std::ostream& operator<<(std::ostream& os, const core* element);
}; 

#endif