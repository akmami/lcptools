#ifndef   CORE_CPP
#define   CORE_CPP

#include <deque>
#include "base_core.cpp"

namespace lcp {

	struct core {
		
		// Represenation related variables
		uint label;
		uchar label_length;

		// Core related variables
		int start;
		int end;

		void compress(const core* other) {

			uint t_label = this->label, o_label = other->label;
			uchar bit_count = this->label_length > other->label_length ? other->label_length : this->label_length;
			uint new_label = 0;
			
			while (bit_count-- > 0 && t_label % 2 == o_label % 2) {
				t_label /= 2;
				o_label /= 2;
				new_label++;
			}
			
			// shift left by 1 bit and set last bit to difference
			new_label = 2 * new_label + t_label % 2;
			
			// Compressed value is: new_label

			// Change this object according to  the new values represents compressed version.
			this->label = new_label;
			this->label_length = 0;

			while (new_label) {
				this->label_length++;
				new_label /= 2;
			}
			
			this->label_length = this->label_length > 2 ? this->label_length : 2;
		}
		
		core(std::deque<lcp::base_core*>::iterator it1, std::deque<lcp::base_core*>::iterator it2) {
			
			this->start = (*it1)->start;
			this->end = (*(it2-1))->end;

			this->label = 0;
			this->label_length = 0;

			// Concatinating cores
		    uchar index = 0;
		    
		    for( std::deque<base_core*>::iterator it = it2-1; it != it1-1; it-- ) {
				this->label |= (*it)->label() << index;
				index += (*it)->block_number * SIZE_PER_BLOCK - (*it)->start_index;
			}

			this->label_length = index > 31 ? 32 : index + 1;
		}

		core(std::deque<core*>::iterator it1, std::deque<core*>::iterator it2) {
			this->start = (*it1)->start;
			this->end = (*(it2-1))->end;
			
			this->label = 0;
			this->label_length = 0;
			
			// Concatinating cores		    
		    for( std::deque<core*>::iterator it = it2-1; it != it1-1; it-- ) {
				this->label |= (*it)->label << this->label_length;
				this->label_length += (*it)->label_length;
			}

			this->label_length = this->label_length > 31 ? 32 : this->label_length;
		}

		core(uint label, uchar label_length, int start, int end) {
			this->label = label;
			this->label_length = label_length;
			this->start = start;
			this->end = end;
		}
	};


	// core operator overloads
	bool operator == (const core& lhs, const core& rhs) {
	    return lhs.label == rhs.label;
	};

	bool operator > (const core& lhs, const core& rhs) {
	    return lhs.label > rhs.label;
	};

	bool operator < (const core& lhs, const core& rhs) {
	    return lhs.label < rhs.label;
	};

	bool operator != (const core& lhs, const core& rhs) {
	    return lhs.label != rhs.label;
	};

	bool operator >= (const core& lhs, const core& rhs) {
		return lhs.label >= rhs.label;
	};

	bool operator <= (const core& lhs, const core& rhs) {
	    return lhs.label <= rhs.label;
	};

	std::ostream& operator<<(std::ostream& os, const core& element) {
		uchar bit_size = element.label_length;
		while (bit_size--) {
			os << ( (element.label >> bit_size) & 1 );
		}
	    return os;
	};

	std::ostream& operator<<(std::ostream& os, const core* element) {
		uchar bit_size = element->label_length;
		while (bit_size--) {
			os << ( (element->label >> bit_size) & 1 );
		}
	    return os;
	};

}; 

#endif