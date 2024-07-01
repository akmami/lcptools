#ifndef   CORE_H
#define   CORE_H

#include <vector>

namespace lcp {

	class core {
	public:
		// Represenation related variables
		uint label;
		uchar label_length;

		// Core related variables
		int start;
		int end;

		void compress(const core* other);
		
		core(std::vector<lcp::base_core*>::iterator it1, std::vector<lcp::base_core*>::iterator it2);
		core(std::vector<core*>::iterator it1, std::vector<core*>::iterator it2);
		core(uint label, uchar label_length, int start, int end);
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