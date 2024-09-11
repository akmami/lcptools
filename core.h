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
		#ifdef STATS
		// Core related variables
		size_t start;
		size_t end;
		#endif

		// Represenation related variables
		size_t size;
		ublock* p;

		core(std::string::iterator begin, std::string::iterator end, size_t begin_index, size_t end_index, bool rev_comp = false);
		core(std::vector<core*>::iterator begin, std::vector<core*>::iterator end);
		core(ublock* p, size_t size, size_t start, size_t end);
		core(std::ifstream& in);
		~core();

		void compress(const core* other);
		void write(std::ofstream& out) const;
		size_t memsize() const;
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