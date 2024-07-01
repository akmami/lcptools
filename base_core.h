#ifndef   BASE_CORE_H
#define   BASE_CORE_H

#include <vector>

namespace lcp {
	class base_core {
    public:
        // Represenation related variables
		uchar* p;
		int block_number;
		int start_index;

		// Core related variables
		int start;
		int end;
		
		template<typename Iter>
		base_core(Iter it1, Iter it2, int start, bool rev_comp = false);
		base_core(std::vector<base_core*>::iterator it1, std::vector<base_core*>::iterator it2);
		base_core(uchar* p, int block_number, int start_index);
		~base_core();

        bool get(int index) const;
		uint compress(const base_core* other);
		uint label();
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