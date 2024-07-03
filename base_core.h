#ifndef   BASE_CORE_H
#define   BASE_CORE_H

#include <vector>
#include <fstream>
#include <iterator>
#include "constant.h"
#include "encoding.h"

namespace lcp {
	class base_core {
    public:
        // Core related variables
		int start;
		int end;

		// Represenation related variables
		int block_number;
		int start_index;
		uchar* p;

		base_core(std::string::iterator it1, std::string::iterator it2, int start, bool rev_comp = false);
		base_core(std::vector<base_core*>::iterator it1, std::vector<base_core*>::iterator it2);
		base_core(uchar* p, int block_number, int start_index, int start, int end);
		base_core(std::ifstream& in);
		~base_core();

        bool get(int index) const;
		uint compress(const base_core* other);
		uint label();
		void write(std::ofstream& out);
	};

    // base_core operator overloads
	bool operator == (const base_core& lhs, const base_core& rhs);
	bool operator > (const base_core& lhs, const base_core& rhs);
	bool operator < (const base_core& lhs, const base_core& rhs);
	bool operator != (const base_core& lhs, const base_core& rhs);
	bool operator >= (const base_core& lhs, const base_core& rhs);
	bool operator <= (const base_core& lhs, const base_core& rhs);

	std::ostream& operator<<(std::ostream& os, const base_core& element);
	std::ostream& operator<<(std::ostream& os, const base_core* element);
}; 

#endif