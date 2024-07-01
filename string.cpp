#include "string.h"

namespace lcp {

    string::string(std::string &str, bool rev_comp) {

        this->level = 1;
        this->start_index = 0;

        std::string::iterator it2;
        

        this->base_cores.reserve( str.size() / CONSTANT_FACTOR );       // As each core appears with average distance of 2.27,
                                                                        // and the increase is always above the 2, it makes sense to reserve
                                                                        // half of the size for the cores to prevent expansion of vector.
        
        if ( rev_comp ) {
            process_string(str.rbegin(), str.rbegin(), str.rend());
        } else {
            process_string(str.begin(), str.begin(), str.end());
        }
    }

    template<typename Iterator>
    void string::process_string(Iterator it1, Iterator it2, Iterator end, bool rev_comp) {

        int* coefficientsArray = ( rev_comp ? reverse_complement_coefficients : coefficients);

        int index = 0;

        // Find lcp cores
        for ( ; it1 + 2 <= end; it1++, index++ ) {

            if ( coefficientsArray[static_cast<unsigned char>(*it1)] == -1 || coefficientsArray[static_cast<unsigned char>(*it1+1)] == -1 || coefficientsArray[static_cast<unsigned char>(*it1)] == coefficientsArray[static_cast<unsigned char>(*(it1 + 1))] ) {
                continue;
            }
            
            // If there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
            if ( coefficientsArray[static_cast<unsigned char>(*(it1 + 1))] == coefficientsArray[static_cast<unsigned char>(*(it1 + 2))] ) {
                
                for ( it2 = it1 + 3; it2 != end && *(it2) == *(it2 - 1); it2++ ) {}
                
                if ( it2 == end ) {
                    break;
                }

                // If z is invalid character, then do not processed
                if ( coefficientsArray[static_cast<unsigned char>(*it2)] == -1 ) {
                    continue;
                }

                // Check whether there is any invalid character encountered
                if ( coefficientsArray[static_cast<unsigned char>(*(it1+1))] == -1 || coefficientsArray[static_cast<unsigned char>(*(it2-1))] == -1 ) {
                    continue;
                }

                if ( coefficientsArray[static_cast<unsigned char>(*it1)] != -1)
                it2++;
                base_core *new_core = new base_core(it1, it2, index, rev_comp);
                this->base_cores.push_back(new_core);
                
                continue;
            }

            // If there is no subsequent characters such as uxyzv where x!=y and y!=z, check local maxima/minima cases
            if ( it1 + 4 < end ) {
                
                // It is enough to validate y as u and x has already been checked. If y is invalid, then it will be local minima.
                // If z is invalid, then, y cannot be local minima or local maxima not having local minima neighbours.
                // Here, z is ignored.
                if ( coefficientsArray[static_cast<unsigned char>(*(it1 + 2))] == -1 ) {
                    continue;
                }
                
                if (
                    ( coefficientsArray[static_cast<unsigned char>(*(it1 + 2))] < coefficientsArray[static_cast<unsigned char>(*(it1 + 3))] && coefficientsArray[static_cast<unsigned char>(*(it1 + 2))] < coefficientsArray[static_cast<unsigned char>(*(it1 + 1))] ) ||   // local minima
                    ( coefficientsArray[static_cast<unsigned char>(*(it1 + 2))] > coefficientsArray[static_cast<unsigned char>(*(it1 + 3))] && coefficientsArray[static_cast<unsigned char>(*(it1 + 2))] > coefficientsArray[static_cast<unsigned char>(*(it1 + 1))] &&     // local maxima without immediate local minima neighbours
                    coefficientsArray[static_cast<unsigned char>(*(it1))] <= coefficientsArray[static_cast<unsigned char>(*(it1 + 1))] && coefficientsArray[static_cast<unsigned char>(*(it1 + 4))] <= coefficientsArray[static_cast<unsigned char>(*(it1 + 3))] )
                ) {
                    
                    base_core *new_core = new base_core(it1+1, it1+4, index+1, rev_comp);
                    this->base_cores.push_back(new_core);
                }
            }
        }
    }

    string::~string() {
        for ( std::vector<core*>::iterator it = this->cores.begin(); it != this->cores.end(); it++ ) {
            delete *it;
        }
        this->cores.clear();

        for ( std::vector<base_core*>::iterator it = this->base_cores.begin(); it != this->base_cores.end(); it++ ) {
            delete *it;
        }
        this->base_cores.clear();
    }

    bool string::deepen() {
        
        // Compress cores

        // If the lcp level is 1, then the cores are stored in base_cores
        // Hence, it requires seperate handling
        if ( this->level == 1 ) {

            this->cores.reserve( this->base_cores.size() / CONSTANT_FACTOR );
            
            // At least 2 cores are needed for compression
            if ( this->base_cores.size() < 2 ) {
                for ( std::vector<base_core*>::iterator it = this->base_cores.begin(); it != this->base_cores.end(); it++ ) {
                    delete *it;
                }
                this->base_cores.clear();
                this->level++;
                return false;
            }

            std::vector<base_core*>::iterator it_curr = this->base_cores.begin() + 1, it_left = this->base_cores.begin();

            for( ; it_curr != this->base_cores.end(); it_curr++, it_left++ ) {
                unsigned int index = (*it_curr)->compress(*it_left);

                // Determine bit count required to represent new core
                uchar bit_size = 0;
                unsigned int temp = index;
                
                while(temp != 0) {
                    bit_size++;
                    temp /= 2;
                }
                bit_size = bit_size > 2 ? bit_size : 2;

                // Create new core
                lcp::core* new_core = new core( index, bit_size, (*it_curr)->start, (*it_curr)->end );
                this->cores.push_back(new_core);
            }

            // Cores are now represented by cores, hence, no need for base cores
            for ( std::vector<base_core*>::iterator it = this->base_cores.begin(); it != this->base_cores.end(); it++ ) {
                delete *it;
            }
            this->base_cores.clear();
        } 

        int compression_iteratin_index = 0;

        if (this->level == 1) {     // If it is first level, then compression is already done once above
            compression_iteratin_index++;
        }

        for( ; compression_iteratin_index < COMPRESSION_ITERATION_COUNT; compression_iteratin_index++ ) {
            
            // At least 2 cores are needed for compression
            if (this->cores.size() < 2) {
                for ( std::vector<core*>::iterator it = this->cores.begin(); it != this->cores.end(); it++ ) {
                    delete *it;
                }
                this->cores.clear();
                this->level++;
                return false;
            }

            std::vector<core*>::iterator it_curr = this->cores.end() - 1, it_left = this->cores.end() - 2;

            for( ; it_curr > cores.begin() + start_index; it_curr--, it_left-- ) {
                (*it_curr)->compress(*it_left);
            }

            this->start_index++;
        }

        // Find cores from compressed cores.
        std::vector<core*>::iterator it1, it2;
        int index = 0;

        std::vector<core*> temp_cores;

        it1 = this->cores.begin() + start_index;
        
        while( it1 + 1 < this->cores.end() && *(*(it1)) == *(*(it1+1)) ) {
            it1++;
            index++;
        }

        while ( index < COMPRESSION_ITERATION_COUNT && it1 - 1 < this->cores.begin() + start_index ) {
            it1++;
            index++;
        }

        for ( ; it1 + 2 < this->cores.end(); it1++, index++) {

            // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
            if ( *(*(it1)) != *(*(it1 + 1)) && *(*(it1 + 1)) == *(*(it1 + 2)) ) {

                for ( it2 = it1 + 3; it2 < this->cores.end() && *(*(it2)) == *(*(it2 - 1)); it2++ ) {}
                
                if ( it2 < this->cores.end() && (*it2)->end - (*(it1))->start < MAXIMUM_LENGTH ) {
                    it2++;
                    core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it2);
                    temp_cores.push_back(new_core);
                    continue;
                } 
            }

            if (it1 + 3 < this->cores.end()) {

                // if there is no subsequent characters such as xyzuv where z!=y and y!=z and z!=u and u!=v
                if (
                    ( *(*(it1 + 1)) < *(*(it1 + 2)) && *(*(it1 + 1)) < *(*(it1)) ) ||     // local minima
                    ( *(*(it1 + 1)) > *(*(it1 + 2)) && *(*(it1 + 1)) > *(*(it1)) &&       // local maxima without immediate local minima neighbours
                    *(*(it1-1)) <= *(*(it1)) && *(*(it1 + 3)) <= *(*(it1 + 2)) )    
                ) { 
                    if ( (*(it1 + 2))->end - (*(it1 - COMPRESSION_ITERATION_COUNT))->start < MAXIMUM_LENGTH ) {
                        core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it1 + 3);
                        temp_cores.push_back(new_core);
                    }
                }
            }
        }

        for ( std::vector<core*>::iterator it = this->cores.begin(); it != this->cores.end(); it++ ) {
            delete *it;
        }
        this->cores.clear();

        this->cores = temp_cores;
        this->start_index = 0;

        this->level++;
        
        return true;
    }

    bool string::deepen(int lcp_level) {

        if ( lcp_level <= this->level ) {
            return false;
        }

        while ( this->level < lcp_level ) {
            this->deepen();
        }

        return true;
    }


    std::ostream& operator<<(std::ostream& os, const string& element) {
        os << "Level: " << element.level << std::endl;
        if (element.level == 1) {
            for (base_core* c : element.base_cores) {
                os << c << " ";
            }
        } else {
            for (core* c : element.cores) {
                os << c << " ";
            }
        }
        return os;
    };

    std::ostream& operator<<(std::ostream& os, const string* element) {
        os << "Level: " << element->level << std::endl;
        if (element->level == 1) {
            for (base_core* c : element->base_cores) {
                os << c << " ";
            }
        } else {
            for (core* c : element->cores) {
                os << c << " ";
            }
        }
        return os;
    };
};