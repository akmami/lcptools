#ifndef   STRING_CPP
#define   STRING_CPP

typedef unsigned char                   uchar;

#define SIZE_PER_BLOCK                  8
#define COMPRESSION_ITERATION_COUNT     2
#define CORE_LENGTH                     5


#include <iostream>
#include <string>
#include <vector>
#include "encoding.cpp"
#include "core.cpp"

namespace lcp {

    struct string {

        std::vector<core*> cores;
        int level;

        string(std::string &str, int core_length=CORE_LENGTH) {
            
            this->level = 1;

            std::string::iterator it1, it2;
            int max_value, min_value;

            it1 = str.begin();
            
            while(it1 != str.end() && it1+1 != str.end() && *it1 == *(it1+1) ) {
                it1++;
            }

            for ( ; it1 + core_length <= str.end(); it1++) {
                
                // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
                if (coefficients[*(it1 + 1)] == coefficients[*(it1 + 2)]) {
                    for (it2 = it1 + 3; it2 != str.end() && *(it2) == *(it2 - 1); it2++) {}
                    
                    if (it2 == str.end()) {
                        break;
                    }
                    it2++;
                    core *new_core = new core(it1 - str.begin(), it2 - it1, std::string(it1, it2));
                    this->cores.push_back(new_core);
                    it1 = it2 - 3;
                    
                    continue;
                }
                
                // if there is no subsequent characters such as xyzuv where z!=y and y!=z and z!=u and u!=v
                min_value = coefficients[*it1];
                max_value = min_value;

                for (it2 = it1 + 1; it2 != it1 + core_length; it2++) {
                    if (*(it2) == *(it2-1)) {
                        break;
                    }
                    if (min_value > coefficients[*it2]) {
                        min_value = coefficients[*it2];
                    }
                    if (max_value < coefficients[*it2]) {
                        max_value = coefficients[*it2];
                    }
                }
                
                if (it2 == it1 + core_length && 
                    (
                        min_value == coefficients[*(it1 + core_length / 2 ) ] ||            // local minima
                        (
                            max_value == coefficients[*(it1 + core_length / 2)] &&          // local maxima without immediate local minima neighbours
                            min_value != coefficients[*(it1 + core_length / 2 - 1)] && 
                            min_value != coefficients[*(it1 + core_length / 2 + 1)] 
                        ) 
                    ) 
                ) {
                    if ( min_value == -1 ) {
                        continue;
                    }
                    core *new_core = new core(it1 - str.begin(), it2 - it1, std::string(it1, it2));
                    this->cores.push_back(new_core);
                    it1 = it1 + core_length / 2 - 1;
                }
            }
        }

        ~string() {
            while (!cores.empty()) {
                delete cores.back();
                cores.pop_back();
            }
        }

        bool deepen(int core_length=CORE_LENGTH, bool verbose=false) {

            // Compress cores
            verbose && std::cout << "Compressing cores." << std::endl;
            
            for(int i=0; i < COMPRESSION_ITERATION_COUNT; i++) {
                int max_bit_length = 0;
                verbose && std::cout << "Compression iteration " << i + 1 <<  std::endl;
                if (cores.size() < 2)
                    return false;

                std::vector<core*>::iterator it_curr = cores.begin() + 1, it_prev = cores.begin();

                core* uncompressed_bits = (*it_curr)->compress(*it_prev);

                if (max_bit_length < (*it_curr)->block_number * SIZE_PER_BLOCK - (*it_curr)->start_index)
                    max_bit_length = (*it_curr)->block_number * SIZE_PER_BLOCK - (*it_curr)->start_index;
                
                core* temp;
                it_prev = it_curr;
                it_curr++;

                for( ; it_curr != cores.end(); it_curr++ ) {
                    temp = (*it_curr)->compress(uncompressed_bits);

                    delete uncompressed_bits;
                    uncompressed_bits = temp;

                    if (max_bit_length < (*it_curr)->block_number * SIZE_PER_BLOCK - (*it_curr)->start_index)
                        max_bit_length = (*it_curr)->block_number * SIZE_PER_BLOCK - (*it_curr)->start_index;

                    it_prev = it_curr;
                }

                temp = NULL;
                delete uncompressed_bits;
                uncompressed_bits = NULL;

                delete *(cores.begin());
                cores.erase(cores.begin());

                verbose && std::cout << "Compressed. Max length is: " << max_bit_length << std::endl;

              
                for (core* c : this->cores) {
                    verbose && std::cout << c << " ";
                }
                verbose && std::cout << std::endl << std::endl;
                
            }

            verbose && std::cout << "Compressing cores completed." << std::endl;

            if (verbose) { for (core *c : this->cores) { std::cout << *c << " "; }; std::cout << std::endl; }

            verbose && std::cout << "Finding new cores." << std::endl;

            // Find cores from compressed cores.
            std::vector<core*>::iterator it1, it2;
            core *max_value, *min_value;

            std::vector<core*> temp_cores;

            it1 = this->cores.begin();
            
            while(it1 != this->cores.end() && it1+1 != this->cores.end() && *(*it1) == *(*(it1+1)) ) {
                it1++;
            }

            for ( ; it1 + core_length <= this->cores.end(); it1++) {
                
                // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
                if ( *(*(it1 + 1)) == *(*(it1 + 2)) ) {
                    for (it2 = it1 + 3; it2 != this->cores.end() && *(*(it2)) == *(*(it2 - 1)); it2++) {}
                    
                    if (it2 == this->cores.end()) {
                        break;
                    }
                    it2++;
                    core *new_core = new core(*it1, *(it1+1), *(it1+2), *(it1+3), *(it1+4));
                    temp_cores.push_back(new_core);
                    it1 = it2 - 3;
                    
                    continue;
                }
                
                // if there is no subsequent characters such as xyzuv where z!=y and y!=z and z!=u and u!=v
                min_value = (*it1);
                max_value = min_value;

                for (it2 = it1 + 1; it2 != it1 + core_length; it2++) {
                    if ( *(*(it2)) == *(*(it2-1)) ) {
                        break;
                    }
                    if (*min_value > *(*it2)) {
                        min_value = (*it2);
                    }
                    if (*max_value < *(*it2)) {
                        max_value = (*it2);
                    }
                }
                
                if (it2 == it1 + core_length && 
                    (
                        *min_value == *(*(it1 + core_length / 2 )) ||            // local minima
                        (
                            *max_value == *(*(it1 + core_length / 2)) &&          // local maxima without immediate local minima neighbours
                            *min_value != *(*(it1 + core_length / 2 - 1)) && 
                            *min_value != *(*(it1 + core_length / 2 + 1))
                        ) 
                    ) 
                ) {
                    core *new_core = new core(*it1, *(it1+1), *(it1+2), *(it1+3), *(it1+4));
                    temp_cores.push_back(new_core);
                    it1 = it1 + core_length / 2 - 1;
                }
            }

            max_value = NULL;
            min_value = NULL;

            while (!this->cores.empty()) {
                delete this->cores.back();
                this->cores.pop_back();
            }

            if (verbose) { for (core *c : temp_cores) { std::cout << *c << " "; } ; std::cout << std::endl; }

            this->cores.insert(this->cores.end(), temp_cores.begin(), temp_cores.end());
            temp_cores.erase(temp_cores.begin(), temp_cores.end());

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