#include "core.h"

namespace lcp {
	
	core::core(std::string::iterator begin, std::string::iterator end, size_t start_index, size_t end_index, bool rev_comp) {

		int* coefficientsArray = ( rev_comp ? reverse_complement_coefficients : coefficients);

		this->start = start_index;
		this->end = end_index;

		this->block_number = ( (end_index - start_index) * dict_bit_size - 1) / SIZE_PER_BLOCK + 1 ;
		this->start_index = this->block_number * SIZE_PER_BLOCK - (end_index - start_index) * dict_bit_size;

		// make allocation for the bit representation
		// std::cout << "block_number: " << this->block_number << std::endl;
		// std::cout << "start_index: " << start_index << std::endl;
		// std::cout << "end_index: " << end_index << std::endl;
		this->p = new ublock[this->block_number];

		// clear dumps
		for (size_t i = 0; i < this->block_number; i++) {
			this->p[i] = 0;
		}

		// encoding string to bits
		int coefficient, shift;
		size_t index = this->start_index, block_index;

		for (std::string::iterator it = begin; it != end; it++) {
			coefficient = coefficientsArray[static_cast<unsigned char>(*it)];
			block_index = index / SIZE_PER_BLOCK;
			shift = SIZE_PER_BLOCK - ( index % SIZE_PER_BLOCK ) - dict_bit_size;

			if (shift >= 0) {
				this->p[block_index] |= (coefficient << shift);
			} else {
				this->p[block_index] |= (coefficient >> (-shift));
				if (block_index + 1 < this->block_number) {
					this->p[block_index + 1] |= (coefficient << (SIZE_PER_BLOCK + shift));
				}
			}
			index += dict_bit_size;
		}

		coefficientsArray = nullptr;
	};

	core::core(std::vector<core*>::iterator begin, std::vector<core*>::iterator end) {
		this->start = (*begin)->start;
		this->end = (*(end-1))->end;

		// calculate required number of bits to represent core
		size_t bit_size = 0;
		for ( std::vector<core*>::iterator it = begin; it != end; it++ ) {
			bit_size += (*it)->block_number * SIZE_PER_BLOCK - (*it)->start_index;
		}
		
		this->block_number = (bit_size - 1) / SIZE_PER_BLOCK + 1;
		this->start_index = this->block_number * SIZE_PER_BLOCK - bit_size;
		
		// make allocation for the bit representation
		this->p = new ublock[this->block_number];

		// clear dumps
		for( size_t i = 0; i < this->block_number; i++ ) {
			this->p[i] = 0;
		}

		size_t index = this->start_index, block_index, block_size;
		int block, shift;

		for( std::vector<core*>::iterator it = begin; it < end; it++ ) {

			for ( size_t i = 0; i < (*it)->block_number; i++ ) {
				block_index = index / SIZE_PER_BLOCK;
				block = (*it)->p[i];
				block_size = ( i == 0 ? SIZE_PER_BLOCK - (*it)->start_index : SIZE_PER_BLOCK );
				shift = SIZE_PER_BLOCK - ( index % SIZE_PER_BLOCK ) - block_size;

				// shift and paste
				
				// if after shifting, there is overflow
				// always overflow occurs if block index is not 0 or if so, 
				// number of bits in first block + index is causing overflow.
				// std::cout << "index: " << index << ", block: " << block << ", block_size: " << block_size << ", shift: " << shift << std::endl;
				if (shift >= 0) {
					this->p[block_index] |= (block << shift);
				} else {
					this->p[block_index] |= (block >> (-shift));
					if (block_index + 1 < this->block_number) {
						this->p[block_index + 1] |= (block << (SIZE_PER_BLOCK + shift));
					}
				}
				index += block_size;
			}
		}
	};

	core::core(ublock* p, size_t block_number, size_t start_index, size_t start, size_t end) {
		this->p = p;
		this->block_number = block_number;
		this->start_index = start_index;
		this->start = start;
		this->end = end;
	};

	core::core(std::ifstream& in) {
		in.read(reinterpret_cast<char*>(&start), sizeof(start));
        in.read(reinterpret_cast<char*>(&end), sizeof(end));
        in.read(reinterpret_cast<char*>(&block_number), sizeof(block_number));
        in.read(reinterpret_cast<char*>(&start_index), sizeof(start_index));
		this->p = new ublock[this->block_number];

		for( size_t i = 0; i < this->block_number; i++ ) {
			this->p[i] = 0;
		}

        in.read(reinterpret_cast<char*>(p), this->block_number * sizeof(unsigned char));
    }

	core::~core() {
		delete[] p;
		this->p = nullptr;
	};

	// delete in near future
	uint core::label() {
		uint label = 0;
		for (int i = this->block_number - 1, index = 0; i >= 0 && index < 32; i--, index += SIZE_PER_BLOCK) {
			if (index + SIZE_PER_BLOCK > 31) {
				label |= (this->p[i] << index) & ((1u << (32 - index)) - 1);
				break;
			}
			label |= (this->p[i] << index);
		}
		return label;
	};

	bool core::get(int index) const {
		return ( this->p[(this->start_index + index) / SIZE_PER_BLOCK] & ( 1 << ( SIZE_PER_BLOCK - ( (this->start_index + index) % SIZE_PER_BLOCK ) - 1 ) ) );
	};

	void core::compress(const core* other) {
		
		size_t o_block_index = other->block_number - 1, t_block_index = this->block_number - 1;
		ublock o = other->p[o_block_index], t = this->p[t_block_index];
		size_t current_index = 0, new_bit_size = 0, temp = 0;
		
		while (o_block_index > 0 && t_block_index > 0 && o == t) {
			o = other->p[--o_block_index];
			t = this->p[--t_block_index];
		}

		current_index = o_block_index ? 
			(t_block_index ? 
				0 : 
				this->start_index) : 
			(t_block_index ? 
				other->start_index :
				std::max(other->start_index, this->start_index) );
		

		while (current_index < SIZE_PER_BLOCK && o % 2 == t % 2) {
			o /= 2;
			t /= 2;
			current_index++;
			temp++;
		}
		
		// shift left by 1 bit and set last bit to difference
		size_t new_label = 2 * ( (this->block_number - t_block_index - 1) * SIZE_PER_BLOCK + temp) + t % 2;

		// count bits requred to reperesent new_label
		new_bit_size = 0;
		temp = new_label;
		while(temp != 0) {
			new_bit_size++;
			temp /= 2;
		}

		new_bit_size = new_bit_size > 2 ? new_bit_size : 2;

		// compressed value is: new_label
		this->block_number = (new_bit_size - 1) / SIZE_PER_BLOCK + 1;
		this->start_index = this->block_number * SIZE_PER_BLOCK - new_bit_size;
		delete[] this->p;
		this->p = nullptr;

		// make allocation for the bit representation
		this->p = new ublock[this->block_number];

		// clear old dumps
		for(size_t i=0; i<this->block_number; i++) {
			this->p[i] = 0;
		}

		// Set bits block by block and avoid unnecesary assignments
		int current_block = this->block_number - 1;

		while ( new_label > 0 ) {
			this->p[current_block] = (ublock)new_label;
			new_label = new_label >> SIZE_PER_BLOCK;
			current_block--;
		}
	};

    void core::write(std::ofstream& out) {
		out.write(reinterpret_cast<const char*>(&start), sizeof(start));
        out.write(reinterpret_cast<const char*>(&end), sizeof(end));
        out.write(reinterpret_cast<const char*>(&block_number), sizeof(block_number));
        out.write(reinterpret_cast<const char*>(&start_index), sizeof(start_index));
        out.write(reinterpret_cast<char*>(p), block_number * sizeof(unsigned char));
    }

	// core operator overloads
	bool operator == (const core& lhs, const core& rhs) {
	    size_t lhs_block_index = 0;
		size_t rhs_block_index = 0;
		
		if (lhs.block_number < rhs.block_number) {
			while(rhs.block_number - rhs_block_index != lhs.block_number) {
				if (rhs.p[rhs_block_index] > 0) {
					return false;
				}
				rhs_block_index++;
			}
		}
		else if (lhs.block_number > rhs.block_number) {
			while(lhs.block_number - lhs_block_index != rhs.block_number) {
				if (lhs.p[lhs_block_index] > 0) {
					return true;
				}
				lhs_block_index++;
			}
		}

	    while( lhs_block_index < lhs.block_number ) {
	    	if ( lhs.p[lhs_block_index] != rhs.p[rhs_block_index] ) {
	    		return false;
	    	}
	    	lhs_block_index++;
	    	rhs_block_index++;
	    }
	    return true;
	};

	bool operator > (const core& lhs, const core& rhs) {
		size_t lhs_block_index = 0;
		size_t rhs_block_index = 0;
		
		if (lhs.block_number < rhs.block_number) {
			while(rhs.block_number - rhs_block_index != lhs.block_number) {
				if (rhs.p[rhs_block_index] > 0) {
					return false;
				}
				rhs_block_index++;
			}
		}
		else if (lhs.block_number > rhs.block_number) {
			while(lhs.block_number - lhs_block_index != rhs.block_number) {
				if (lhs.p[lhs_block_index] > 0) {
					return true;
				}
				lhs_block_index++;
			}
		}

	    while( lhs_block_index < lhs.block_number ) {
	    	if ( lhs.p[lhs_block_index] > rhs.p[rhs_block_index] ) {
	    		return true;
	    	}
	    	if ( lhs.p[lhs_block_index] < rhs.p[rhs_block_index] ) {
	    		return false;
	    	}
	    	lhs_block_index++;
	    	rhs_block_index++;
	    }
	    return false;
	};

	bool operator < (const core& lhs, const core& rhs) {
	    size_t lhs_block_index = 0;
		size_t rhs_block_index = 0;
		
		if (lhs.block_number < rhs.block_number) {
			while(rhs.block_number - rhs_block_index != lhs.block_number) {
				if (rhs.p[rhs_block_index] > 0) {
					return true;
				}
				rhs_block_index++;
			}
		}
		else if (lhs.block_number > rhs.block_number) {
			while(lhs.block_number - lhs_block_index != rhs.block_number) {
				if (lhs.p[lhs_block_index] > 0) {
					return false;
				}
				lhs_block_index++;
			}
		}

	    while( lhs_block_index < lhs.block_number ) {
	    	if ( lhs.p[lhs_block_index] < rhs.p[rhs_block_index] ) {
	    		return true;
	    	}
	    	if ( lhs.p[lhs_block_index] > rhs.p[rhs_block_index] ) {
	    		return false;
	    	}
	    	lhs_block_index++;
	    	rhs_block_index++;
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
		for (int block_index = element.block_number - 1; 0 <= block_index; block_index-- ){
			for (size_t index = element.start_index; index < element.block_number * SIZE_PER_BLOCK; index++ ) {
				os << ((element.p[index / SIZE_PER_BLOCK] >> (SIZE_PER_BLOCK - index % SIZE_PER_BLOCK)) & 1);
			}
		}
	    return os;
	};

	std::ostream& operator<<(std::ostream& os, const core* element) {
		int bit_size = element->block_number * SIZE_PER_BLOCK - element->start_index;
		for (int index=0; index<bit_size; index++) {
			os << element->get(index);
		}
	    return os;
	};

};