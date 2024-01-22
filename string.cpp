#ifndef   STRING_CPP
#define   STRING_CPP

typedef unsigned char                   uchar;

#define SIZE_PER_BLOCK                  8
#define COMPRESSION_ITERATION_COUNT     1
#define MAXIMUM_LENGTH                  10000


#include <iostream>
#include <string>
#include <deque>
#include "encoding.cpp"
#include "core.cpp"
#include "base_core.cpp"

namespace lcp {

    struct string {

        std::deque<core*> cores;
        std::deque<base_core*> base_cores;

        int level;

        string(std::string &str) {
            
            this->level = 1;

            std::string::iterator it1, it2;
            int min_value;

            it1 = str.begin();
            int index = 0;
            
            while(it1 != str.end() && it1+1 != str.end() && it1+2 != str.end() && *(it1+1) == *(it1+2) ) {
                it1++;
                index++;
            }

            for ( ; it1 + 2 <= str.end(); it1++, index++) {
                
                // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
                if (coefficients[static_cast<unsigned char>(*it1)] != coefficients[static_cast<unsigned char>(*(it1 + 1))] && coefficients[static_cast<unsigned char>(*(it1 + 1))] == coefficients[static_cast<unsigned char>(*(it1 + 2))]) {
                    for (it2 = it1 + 3; it2 != str.end() && *(it2) == *(it2 - 1); it2++) {}
                    
                    if (it2 == str.end()) {
                        break;
                    }

                    // check whether there is any invalid character encountered
                    if ( coefficients[static_cast<unsigned char>(*it1)] == -1 || coefficients[static_cast<unsigned char>(*(it1+1))] == -1 || coefficients[static_cast<unsigned char>(*(it2-1))] == -1) {
                        continue;
                    }

                    it2++;
                    base_core *new_core = new base_core(it1, it2, index);
                    this->base_cores.push_back(new_core);
                    
                    continue;
                }

                if (it1 + 3 > str.end()) {
                    break;
                }
                
                // if there is no subsequent characters such as uxyzv where x!=y and y!=z
                min_value = coefficients[static_cast<unsigned char>(*it1)];

                for (it2 = it1 + 1; it2 != it1 + 3; it2++) {
                    if (min_value > coefficients[static_cast<unsigned char>(*it2)]) {
                        min_value = coefficients[static_cast<unsigned char>(*it2)];
                    }
                }
                
                if (
                    ( coefficients[static_cast<unsigned char>(*(it1 + 2))] < coefficients[static_cast<unsigned char>(*(it1 + 3))] && coefficients[static_cast<unsigned char>(*(it1 + 2))] < coefficients[static_cast<unsigned char>(*(it1 + 1))] ) ||   // local minima
                    ( coefficients[static_cast<unsigned char>(*(it1 + 2))] > coefficients[static_cast<unsigned char>(*(it1 + 3))] && coefficients[static_cast<unsigned char>(*(it1 + 2))] > coefficients[static_cast<unsigned char>(*(it1 + 1))] &&     // local maxima without immediate local minima neighbours
                      coefficients[static_cast<unsigned char>(*(it1))] <= coefficients[static_cast<unsigned char>(*(it1 + 1))] && coefficients[static_cast<unsigned char>(*(it1 + 4))] <= coefficients[static_cast<unsigned char>(*(it1 + 3))] )
                ) {
                    if ( min_value == -1 ) {    // invalid character encountered
                        continue;
                    }
                    base_core *new_core = new base_core(it1+1, it1+4, index+1);
                    this->base_cores.push_back(new_core);
                }
            }
        }

        ~string() {
            while (!cores.empty()) {
                delete cores.front();
                cores.pop_front();
            }
            while (!base_cores.empty()) {
                delete base_cores.front();
                base_cores.pop_front();
            }
        }

        bool deepen() {

            // Compress cores
            if (this->level == 1) {
                if (this->base_cores.size() < 2)
                    return false;

                std::deque<base_core*>::iterator it_curr = this->base_cores.end() - 1, it_left = this->base_cores.end() - 2;

                for( ; it_curr != this->base_cores.begin(); it_curr--, it_left-- ) {
                    unsigned int index = (*it_curr)->compress(*it_left);

                    // determine bit count required to represent new core
                    uchar bit_size = 0;
                    unsigned int temp = index;
                    
                    while(temp != 0) {
                        bit_size++;
                        temp /= 2;
                    }
                    bit_size = bit_size > 2 ? bit_size : 2;

                    // create new core
                    lcp::core* new_core = new core(index, bit_size, (*it_curr)->start, (*it_curr)->end );
                    this->cores.push_front(new_core);
                }

                // cores are now represented by cores, hence, no need for base cores
                while (!base_cores.empty()) {
                    delete base_cores.front();
                    base_cores.pop_front();
                }
            } 

            int i = 0;

            if (this->level == 1) {     // if it is first level, then compression is already done once above
                i++;
            }

            for( ; i < COMPRESSION_ITERATION_COUNT; i++) {
                
                if (this->cores.size() < 2)
                    return false;

                std::deque<core*>::iterator it_curr = this->cores.end() - 1, it_left = this->cores.end() - 2;

                for( ; it_curr != cores.begin(); it_curr--, it_left-- ) {
                    (*it_curr)->compress(*it_left);
                }

                delete this->cores.front();
                this->cores.pop_front();
            }

            // Find cores from compressed cores.
            std::deque<core*>::iterator it1, it2;
            int index = 0;

            std::deque<core*> temp_cores;

            it1 = this->cores.begin();
            
            while( it1 + 1 < this->cores.end() && *(*(it1)) == *(*(it1+1)) ) {
                it1++;
                index++;
            }

            while ( index < COMPRESSION_ITERATION_COUNT && it1 - 1 < this->cores.begin() ) {
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

            while (!this->cores.empty()) {
                delete this->cores.front();
                this->cores.pop_front();
            }

            this->cores = temp_cores;

            this->level++;
            
            return true;
        }

    };

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

#endif