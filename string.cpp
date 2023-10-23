#ifndef   STRING_CPP
#define   STRING_CPP

typedef unsigned char                   uchar;

#define SIZE_PER_BLOCK                  8
#define COMPRESSION_ITERATION_COUNT     2
#define CORE_LENGTH                     5
#define MAXIMUM_LENGTH                  10000


#include <iostream>
#include <string>
#include <deque>
#include "encoding.cpp"
#include "core.cpp"

namespace lcp {

    struct string {

        std::deque<core*> cores;
        int level;

        string(std::string &str, int core_length=CORE_LENGTH) {
            
            this->level = 1;

            std::string::iterator it1, it2;
            int max_value, min_value;

            it1 = str.begin();
            int index = 0;
            
            while(it1 != str.end() && it1+1 != str.end() && it1+2 != str.end() && *(it1+1) == *(it1+2) ) {
                it1++;
                index++;
            }

            for ( ; it1 + 2 <= str.end(); it1++, index++) {
                
                // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
                if (coefficients[*(it1)] != coefficients[*(it1 + 1)] && coefficients[*(it1 + 1)] == coefficients[*(it1 + 2)]) {
                    for (it2 = it1 + 3; it2 != str.end() && *(it2) == *(it2 - 1); it2++) {}
                    
                    if (it2 == str.end()) {
                        break;
                    }
                    it2++;
                    core *new_core = new core(it1, it2, index);
                    this->cores.push_back(new_core);
                    
                    continue;
                }

                if (it1 + core_length > str.end()) {
                    break;
                }
                
                // if there is no subsequent characters such as uxyzv where x!=y and y!=z
                min_value = coefficients[*it1];

                for (it2 = it1 + 1; it2 != it1 + core_length; it2++) {
                    if (min_value > coefficients[*it2]) {
                        min_value = coefficients[*it2];
                    }
                }
                
                if (
                    ( coefficients[*(it1 + 2) ] < coefficients[*(it1 + 3) ] && coefficients[*(it1 + 2) ] < coefficients[*(it1 + 1) ] ) ||   // local minima
                    ( coefficients[*(it1 + 2) ] > coefficients[*(it1 + 3) ] && coefficients[*(it1 + 2) ] > coefficients[*(it1 + 1) ] &&     // local maxima without immediate local minima neighbours
                      coefficients[*(it1) ] <= coefficients[*(it1 + 1) ] && coefficients[*(it1 + 4) ] <= coefficients[*(it1 + 3) ] )
                ) {
                    if ( min_value == -1 ) {
                        continue;
                    }
                    core *new_core = new core(it1+1, it1+4, index+1);
                    this->cores.push_back(new_core);
                }
            }
        }

        ~string() {
            while (!cores.empty()) {
                delete cores.front();
                cores.pop_front();
            }
        }

        bool deepen(int core_length=CORE_LENGTH) {

            // Compress cores
            for(int i=0; i < COMPRESSION_ITERATION_COUNT; i++) {
                int max_bit_length = 0;
                if (cores.size() < 2)
                    return false;

                std::deque<core*>::iterator it_curr = cores.end() - 1, it_left = cores.end()-2;

                for( ; it_curr != cores.begin(); it_curr--, it_left-- ) {
                    (*it_curr)->compress(*it_left);

                    if (max_bit_length < (*it_curr)->block_number * SIZE_PER_BLOCK - (*it_curr)->start_index)
                        max_bit_length = (*it_curr)->block_number * SIZE_PER_BLOCK - (*it_curr)->start_index;
                }

                delete cores.front();
                cores.pop_front();    
            }

            // Find cores from compressed cores.
            std::deque<core*>::iterator it1, it2;
            int index = 0;

            std::deque<core*> temp_cores;

            it1 = this->cores.begin();
            
            while(it1 + 2 < this->cores.end() && *(*(it1+1)) == *(*(it1+2)) ) {
                it1++;
                index++;
            }

            for ( ; it1 + 2 < this->cores.end(); it1++, index++) {
                
                // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
                if ( *(*(it1)) != *(*(it1 + 1)) && *(*(it1 + 1)) == *(*(it1 + 2)) ) {
                    for (it2 = it1 + 3; it2 < this->cores.end() && *(*(it2)) == *(*(it2 - 1)); it2++) {}
                    
                    if (it2 == this->cores.end()) {
                        break;
                    }
                    if ( it1 - COMPRESSION_ITERATION_COUNT < this->cores.begin() ) {
                        continue;
                    }
                    it2++;
                    
                    if ( (*it2)->end - (*it1)->start < MAXIMUM_LENGTH) {
                        core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it2);
                        temp_cores.push_back(new_core);
                    }
                    
                    continue;
                }

                if (it1 + core_length > this->cores.end()) {
                    break;
                }
                
                // if there is no subsequent characters such as xyzuv where z!=y and y!=z and z!=u and u!=v
                
                if (
                    ( *(*(it1 + 2)) < *(*(it1 + 3)) && *(*(it1 + 2)) < *(*(it1 + 1)) ) ||     // local minima
                    ( *(*(it1 + 2)) > *(*(it1 + 3)) && *(*(it1 + 2)) > *(*(it1 + 1)) &&       // local maxima without immediate local minima neighbours
                      *(*(it1)) <= *(*(it1 + 1)) && *(*(it1 + 4)) <= *(*(it1 + 3)) )    
                ) {
                    if ( it1 + 1 - COMPRESSION_ITERATION_COUNT < this->cores.begin() ) {
                        continue;
                    }
                    if ( (*(it1 + 4))->end - (*(it1 + 1 - COMPRESSION_ITERATION_COUNT))->start < MAXIMUM_LENGTH) {
                        core *new_core = new core(it1 + 1 - COMPRESSION_ITERATION_COUNT, it1 + 4);
                        temp_cores.push_back(new_core);
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
        for (core* c : element.cores) {
            os << c << " ";
        }
        return os;
    };

    std::ostream& operator<<(std::ostream& os, const string* element) {
        os << "Level: " << element->level << std::endl;
        for (core* c : element->cores) {
            os << c << " ";
        }
        return os;
    };
};

#endif