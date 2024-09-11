#include "core.h"


inline size_t block_number(size_t size) {
	return ( size - 1 ) / SIZE_PER_BLOCK + 1;
};


inline size_t start_index(size_t size) {
	return SIZE_PER_BLOCK - size % SIZE_PER_BLOCK;
}

namespace lcp {
	
	core::core( std::string::iterator begin, std::string::iterator end, size_t begin_index, size_t end_index, bool rev_comp ) {

		int* coefficientsArray = ( rev_comp ? reverse_complement_coefficients : coefficients);

		#ifdef STATS
		this->start = begin_index;
		this->end = end_index;
		#endif

		this->size = ( end_index - begin_index ) * dict_bit_size;

		// make allocation for the bit representation
		this->p = new ublock[ block_number(this->size) ];

		// clear dumps
		for ( size_t i = 0; i < block_number(this->size); i++ ) {
			this->p[i] = 0;
		}

		// encoding string to bits
		int coefficient, shift;
		size_t index = start_index( this->size ), block_index;

		for (std::string::iterator it = begin; it != end; it++) {
			coefficient = coefficientsArray[static_cast<unsigned char>(*it)];
			block_index = index / SIZE_PER_BLOCK;
			shift = SIZE_PER_BLOCK - ( index % SIZE_PER_BLOCK ) - dict_bit_size;

			if (shift >= 0) {
				this->p[block_index] |= (coefficient << shift);
			} else {
				this->p[block_index] |= (coefficient >> (-shift));
				if (block_index + 1 < block_number(this->size)) {
					this->p[block_index + 1] |= (coefficient << (SIZE_PER_BLOCK + shift));
				}
			}
			index += dict_bit_size;
		}

		coefficientsArray = nullptr;
	};

	core::core(std::vector<core*>::iterator begin, std::vector<core*>::iterator end) {
		#ifdef STATS
		this->start = (*begin)->start;
		this->end = (*(end-1))->end;
		#endif

		// calculate required number of bits to represent core
		this->size = 0;
		for ( std::vector<core*>::iterator it = begin; it != end; it++ ) {
			this->size += (*it)->size;
		}

		// make allocation for the bit representation
		this->p = new ublock[ block_number(this->size) ];
		
		// clear dumps
		for( size_t i = 0; i < block_number(this->size); i++ ) {
			this->p[i] = 0;
		}

		size_t index = start_index( this->size ), block_index, block_size;
		int block, shift;

		for( std::vector<core*>::iterator it = begin; it != end; it++ ) {

			for ( size_t i = 0; i < block_number((*it)->size); i++ ) {
				block_index = index / SIZE_PER_BLOCK;
				block = (*it)->p[i];
				block_size = ( i == 0 ? SIZE_PER_BLOCK - start_index((*it)->size): SIZE_PER_BLOCK );
				shift = SIZE_PER_BLOCK - ( index % SIZE_PER_BLOCK ) - block_size;

				// shift and paste
				// if after shifting, there is overflow
				// always overflow occurs if block index is not 0 or if so, 
				// number of bits in first block + index is causing overflow.
				if (shift >= 0) {
					this->p[block_index] |= (block << shift);
				} else {
					this->p[block_index] |= (block >> (-shift));
					if (block_index + 1 < block_number(this->size)) {
						this->p[block_index + 1] |= (block << (SIZE_PER_BLOCK + shift));
					}
				}
				index += block_size;
			}
		}
	};

	core::core(ublock* p, size_t size, size_t start, size_t end) {
		this->p = p;
		this->size = size;
		#ifdef STATS
		this->start = start;
		this->end = end;
		#endif
	};

	core::core(std::ifstream& in) {
		#ifdef STATS
		in.read(reinterpret_cast<char*>(&start), sizeof(start));
		in.read(reinterpret_cast<char*>(&end), sizeof(end));
		#endif
        in.read(reinterpret_cast<char*>(&size), sizeof(size));
		this->p = new ublock[block_number(this->size)];

		for( size_t i = 0; i < block_number(this->size); i++ ) {
			this->p[i] = 0;
		}

        in.read(reinterpret_cast<char*>(p), block_number(this->size) * sizeof(ublock));
    };

	core::~core() {
		delete[] p;
		this->p = nullptr;
	};

	void core::compress(const core* other) {
		
		size_t o_block_index = block_number(other->size) - 1, t_block_index = block_number(this->size) - 1;
		ublock o = other->p[o_block_index], t = this->p[t_block_index];
		size_t current_index = 0, new_bit_size = 0, temp = 0;
		
		while (o_block_index > 0 && t_block_index > 0 && o == t) {
			o = other->p[--o_block_index];
			t = this->p[--t_block_index];
		}

		current_index = o_block_index ? 
			(t_block_index ? 
				0 : 
				start_index(this->size) ) : 
			(t_block_index ? 
				start_index(other->size) :
				std::max(start_index(other->size), start_index(this->size) ) );
		

		while (current_index < SIZE_PER_BLOCK && o % 2 == t % 2) {
			o /= 2;
			t /= 2;
			current_index++;
			temp++;
		}
		
		// shift left by 1 bit and set last bit to difference
		size_t new_label = 2 * ( (block_number(this->size) - t_block_index - 1) * SIZE_PER_BLOCK + temp) + t % 2;

		// count bits requred to reperesent new_label
		new_bit_size = 0;
		temp = new_label;
		while(temp != 0) {
			new_bit_size++;
			temp /= 2;
		}

		new_bit_size = new_bit_size > 2 ? new_bit_size : 2;

		// compressed value is: new_label
		this->size = new_bit_size;
		delete[] this->p;
		this->p = nullptr;

		// make allocation for the bit representation
		this->p = new ublock[block_number(this->size)];

		// clear old dumps
		for(size_t i=0; i<block_number(this->size); i++) {
			this->p[i] = 0;
		}

		// Set bits block by block and avoid unnecesary assignments
		int current_block = block_number(this->size) - 1;

		while ( new_label > 0 ) {
			this->p[current_block] = (ublock)new_label;
			new_label = new_label >> SIZE_PER_BLOCK;
			current_block--;
		}
	};

    void core::write(std::ofstream& out) const {
		#ifdef STATS
		out.write(reinterpret_cast<const char*>(&start), sizeof(start));
		out.write(reinterpret_cast<const char*>(&end), sizeof(end));
		#endif
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));
        out.write(reinterpret_cast<char*>(p), block_number(this->size) * sizeof(ublock));
    };

	size_t core::memsize() const {
        size_t size = sizeof(core);
        if (this->p != nullptr) {
            size += block_number(this->size) * sizeof(ublock);
        }
        return size;
    };

	// // core operator overloads
	// bool operator == (const core& lhs, const core& rhs) {
	//     size_t lhs_block_index = 0;
	// 	size_t rhs_block_index = 0;
		
	// 	if (block_number(lhs.size) < block_number(rhs.size)) {
	// 		while(block_number(rhs.size) - rhs_block_index != block_number(lhs.size)) {
	// 			if (rhs.p[rhs_block_index] > 0) {
	// 				return false;
	// 			}
	// 			rhs_block_index++;
	// 		}
	// 	}
	// 	else if (block_number(lhs.size) > block_number(rhs.size)) {
	// 		while(block_number(lhs.size) - lhs_block_index != block_number(rhs.size)) {
	// 			if (lhs.p[lhs_block_index] > 0) {
	// 				return true;
	// 			}
	// 			lhs_block_index++;
	// 		}
	// 	}

	//     while( lhs_block_index < block_number(lhs.size) ) {
	//     	if ( lhs.p[lhs_block_index] != rhs.p[rhs_block_index] ) {
	//     		return false;
	//     	}
	//     	lhs_block_index++;
	//     	rhs_block_index++;
	//     }
	//     return true;
	// };

	// bool operator > (const core& lhs, const core& rhs) {
	// 	size_t lhs_block_index = 0;
	// 	size_t rhs_block_index = 0;
		
	// 	if (block_number(lhs.size) < block_number(rhs.size)) {
	// 		while(block_number(rhs.size) - rhs_block_index != block_number(lhs.size)) {
	// 			if (rhs.p[rhs_block_index] > 0) {
	// 				return false;
	// 			}
	// 			rhs_block_index++;
	// 		}
	// 	}
	// 	else if (block_number(lhs.size) > block_number(rhs.size)) {
	// 		while(block_number(lhs.size) - lhs_block_index != block_number(rhs.size)) {
	// 			if (lhs.p[lhs_block_index] > 0) {
	// 				return true;
	// 			}
	// 			lhs_block_index++;
	// 		}
	// 	}

	//     while( lhs_block_index < block_number(lhs.size) ) {
	//     	if ( lhs.p[lhs_block_index] > rhs.p[rhs_block_index] ) {
	//     		return true;
	//     	}
	//     	if ( lhs.p[lhs_block_index] < rhs.p[rhs_block_index] ) {
	//     		return false;
	//     	}
	//     	lhs_block_index++;
	//     	rhs_block_index++;
	//     }
	//     return false;
	// };

	// bool operator < (const core& lhs, const core& rhs) {
	//     size_t lhs_block_index = 0;
	// 	size_t rhs_block_index = 0;
		
	// 	if (block_number(lhs.size) < block_number(rhs.size)) {
	// 		while(block_number(rhs.size) - rhs_block_index != block_number(lhs.size)) {
	// 			if (rhs.p[rhs_block_index] > 0) {
	// 				return true;
	// 			}
	// 			rhs_block_index++;
	// 		}
	// 	}
	// 	else if (block_number(lhs.size) > block_number(rhs.size)) {
	// 		while(block_number(lhs.size) - lhs_block_index != block_number(rhs.size)) {
	// 			if (lhs.p[lhs_block_index] > 0) {
	// 				return false;
	// 			}
	// 			lhs_block_index++;
	// 		}
	// 	}

	//     while( lhs_block_index < block_number(lhs.size) ) {
	//     	if ( lhs.p[lhs_block_index] < rhs.p[rhs_block_index] ) {
	//     		return true;
	//     	}
	//     	if ( lhs.p[lhs_block_index] > rhs.p[rhs_block_index] ) {
	//     		return false;
	//     	}
	//     	lhs_block_index++;
	//     	rhs_block_index++;
	//     }
	//     return false;
	// };

	// core operator overloads
	bool operator == (const core& lhs, const core& rhs) {

		if ( lhs.size != rhs.size ) {
			return false;
		}

		size_t index = 0;

		while ( index < lhs.size ) {
			if ( lhs.p[index / SIZE_PER_BLOCK ] != rhs.p[index / SIZE_PER_BLOCK ] )
				return false;
			
			index += SIZE_PER_BLOCK;
		}

		return true;
	};

	bool operator > (const core& lhs, const core& rhs) {

		if ( lhs.size > rhs.size ) {
			return true;
		} else if ( lhs.size < rhs.size ) {
			return false;
		}

		size_t index = 0;

		while ( index < lhs.size ) {
			if ( lhs.p[index / SIZE_PER_BLOCK ] > rhs.p[index / SIZE_PER_BLOCK ] ) {
				return true;
			} else if ( lhs.p[index / SIZE_PER_BLOCK ] < rhs.p[index / SIZE_PER_BLOCK ] ) {
				return false;
			}
			
			index += SIZE_PER_BLOCK;
		}

		return false;
	};

	bool operator < (const core& lhs, const core& rhs) {
	    if ( lhs.size < rhs.size ) {
			return true;
		} else if ( lhs.size > rhs.size ) {
			return false;
		}

		size_t index = 0;

		while ( index < lhs.size ) {
			if ( lhs.p[index / SIZE_PER_BLOCK ] < rhs.p[index / SIZE_PER_BLOCK ] ) {
				return true;
			} else if ( lhs.p[index / SIZE_PER_BLOCK ] > rhs.p[index / SIZE_PER_BLOCK ] ) {
				return false;
			}
			
			index += SIZE_PER_BLOCK;
		}

		return false;
	};

	bool operator != (const core& lhs, const core& rhs) {
	    return !(lhs == rhs);
	};

	bool operator >= (const core& lhs, const core& rhs) {
		if (lhs < rhs)
			return false;
	    return true;
	};

	bool operator <= (const core& lhs, const core& rhs) {
	    if (lhs > rhs)
	    	return false;
	    return true;
	};

	std::ostream& operator<<(std::ostream& os, const core& element) {
		for (int index = element.size - 1; 0 <= index; index-- ) {
			os << ((element.p[block_number(element.size) - index / SIZE_PER_BLOCK - 1] >> (index % SIZE_PER_BLOCK)) & 1);
		}
	    return os;
	};

	std::ostream& operator<<(std::ostream& os, const core* element) {
		for (int index = element->size - 1; 0 <= index; index-- ) {
			os << ((element->p[block_number(element->size) - index / SIZE_PER_BLOCK - 1] >> (index % SIZE_PER_BLOCK)) & 1);
		}
	    return os;
	};

};