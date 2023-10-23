#ifndef   CORE_CPP
#define   CORE_CPP

#include <deque>

namespace lcp {

	struct core {
		
		// Represenation related variables
		uchar* p;
		int block_number;
		int start_index;

		// Core related variables
		int start;
		int end;

		// element at index:
		// p[(start_index + index) / SIZE_PER_BLOCK] & ( 1 << ( SIZE_PER_BLOCK - ( (start_index + index) % SIZE_PER_BLOCK ) - 1 ) )

		// set to index:
		// p[(start_index + index) / SIZE_PER_BLOCK] |= ( 1 << ( SIZE_PER_BLOCK - ( (start_index + index) % SIZE_PER_BLOCK ) - 1 ) );

		bool get(int index) const {
	    	return ( this->p[(this->start_index + index) / SIZE_PER_BLOCK] & ( 1 << ( SIZE_PER_BLOCK - ( (this->start_index + index) % SIZE_PER_BLOCK ) - 1 ) ) );
		}

		void compress(const core* other) {
			
			int o_block_index = other->block_number - 1, t_block_index = this->block_number - 1;
			uchar o = other->p[o_block_index], t = this->p[t_block_index];
			int current_index = 0, new_bit_size = 0, temp = 0;
			
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
			int index = 2 * ( (this->block_number - t_block_index - 1) * SIZE_PER_BLOCK + temp) + t % 2;
			
			// count bits requred to reperesent index
			new_bit_size = 0;
			temp = index;
			while(temp != 0) {
				new_bit_size++;
				temp /= 2;
			}

			new_bit_size = new_bit_size > 2 ? new_bit_size : 2;

			// Compressed value is: index

			// Change this object according to  the new values represents compressed version.
			this->block_number = (new_bit_size - 1) / SIZE_PER_BLOCK + 1;
			this->start_index = this->block_number * SIZE_PER_BLOCK - new_bit_size;
			this->p = NULL;
			
			// Make allocation for the bit representation
			this->p = (unsigned char *)malloc( this->block_number );

			if (this->p == NULL) {
				throw std::bad_alloc();
			}

			// clear old dumps
			for(int i=0; i<this->block_number; i++) {
				this->p[i] = 0;
			}

			// Set bits block by block and avoid unnecesary assignments
			int current_block = this->block_number - 1;

			while ( index > 0 ) {
				this->p[current_block] = (uchar)index;
				index = index >> SIZE_PER_BLOCK;
				current_block--;
				index -= SIZE_PER_BLOCK;
			}
		}
		core(std::string::iterator it1, std::string::iterator it2, int start) {
			this->start = start;
			this->end = start + (it2 - it1);

			this->block_number = ( (it2 - it1) * dict_bit_size - 1) / SIZE_PER_BLOCK + 1 ;
			this->start_index = this->block_number * SIZE_PER_BLOCK - (it2 - it1) * dict_bit_size;
			
			// Make allocation for the bit representation
			this->p = (unsigned char *)malloc( this->block_number );

			if (this->p == NULL) {
				throw std::bad_alloc();
			}

			// clear dumps
			for( int i=0; i<this->block_number; i++ ) {
				this->p[i] = 0;
			}

			// Encoding string to bits
		    int coefficient, index = 0;
		    
		    for(std::string::iterator char_it = it1; char_it != it2; char_it++) {
		        coefficient = coefficients[*char_it];
		        for (int i = dict_bit_size - 1; i >= 0 ; i--) {
		        	if (coefficient % 2) {
		        		this->p[(this->start_index + index + i) / SIZE_PER_BLOCK] |= ( 1 << ( SIZE_PER_BLOCK - ( (this->start_index + index + i) % SIZE_PER_BLOCK ) - 1 ) );
		        	}
		        	coefficient = coefficient / 2;
		        }
		        index += dict_bit_size;
		    }
		}

		core(std::deque<core*>::iterator it1, std::deque<core*>::iterator it2) {
			this->start = (*it1)->start;
			this->end = (*it2)->end;

			int bit_size = 0;
			for ( std::deque<core*>::iterator it = it1; it != it2; it++ ) {
				bit_size += (*it)->block_number * SIZE_PER_BLOCK - (*it)->start_index;
			}
			
			this->block_number = (bit_size - 1) / SIZE_PER_BLOCK + 1;
			this->start_index = this->block_number * SIZE_PER_BLOCK - bit_size;
			
			// Make allocation for the bit representation
			this->p = (unsigned char *)malloc( this->block_number );

			if (this->p == NULL) {
				throw std::bad_alloc();
			}
			
			// clear dumps
			for( int i=0; i<this->block_number; i++ ) {
				this->p[i] = 0;
			}

			int index = block_number * SIZE_PER_BLOCK - 1;
			for( std::deque<core*>::iterator it = it2-1; it != it1-1; it-- ) {

				for ( int i = (*it)->block_number-1; i >= 0; i--) {
					if ( index > SIZE_PER_BLOCK ){
						this->p[ index / SIZE_PER_BLOCK ] |= (*it)->p[i] << ( SIZE_PER_BLOCK - index % SIZE_PER_BLOCK - 1 );
						if ( index % SIZE_PER_BLOCK != SIZE_PER_BLOCK - 1 ) {
							this->p[ index / SIZE_PER_BLOCK - 1 ] |= (*it)->p[i] >> ( index % SIZE_PER_BLOCK + 1 );
						}			
					} else {
						this->p[ index / SIZE_PER_BLOCK ] |= (*it)->p[i] << ( SIZE_PER_BLOCK - index % SIZE_PER_BLOCK - 1);
					}
					if ( i == 0 ) {
						if ( index > SIZE_PER_BLOCK - (*it)->start_index ) {
							index -= SIZE_PER_BLOCK - (*it)->start_index;
						}
					}
					else {
						index -= SIZE_PER_BLOCK;
					}
				}
			}
		}

		core(uchar* p, int block_number, int start_index) {
			this->p = p;
			this->block_number = block_number;
			this->start_index = start_index;
		}

		~core() {
			if (p != NULL) 
				free(p);
		}
	};


	// core operator overloads
	bool operator == (const core& lhs, const core& rhs) {
	    int lhs_block_index = 0;
		int rhs_block_index = 0;
		
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
		int lhs_block_index = 0;
		int rhs_block_index = 0;
		
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
	    int lhs_block_index = 0;
		int rhs_block_index = 0;
		
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
		int bit_size = element.block_number * SIZE_PER_BLOCK - element.start_index;
		for (int index=0; index<bit_size; index++) {
			os << element.get(index);
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

#endif