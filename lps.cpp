#include "lps.h"

namespace lcp {

    lps::lps(std::string &str, bool rev_comp) {

        this->level = 1;
        this->start_index = 0;

        std::string::iterator it2;
        

        this->cores.reserve( str.size() / CONSTANT_FACTOR );       // As each core appears with average distance of 2.27,
                                                                        // and the increase is always above the 2, it makes sense to reserve
                                                                        // half of the size for the cores to prevent expansion of vector.
        
        if ( rev_comp ) {
            std::reverse(str.begin(), str.end());
        }
        
        parse(str.begin(), str.end(), rev_comp);
    }

    lps::lps(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&level), sizeof(level));
        in.read(reinterpret_cast<char*>(&start_index), sizeof(start_index));
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));

        // read each core object
        
        // resize the vector to the appropriate size
        cores.reserve(size);

        // read each core object
        for (size_t i = 0; i < size; i++) {
            core* new_core = new core(in);
            this->cores.push_back(new_core);
        }
    }

    void lps::parse(std::string::iterator begin, std::string::iterator end, bool rev_comp) {

        int* coefficientsArray = ( rev_comp ? reverse_complement_coefficients : coefficients);

        int index = 0;
        std::string::iterator it1 = begin;
        std::string::iterator it2;

        // Find lcp cores
        for ( ; it1 + 2 < end; it1++, index++ ) {

            // skip invalid character
            if ( coefficientsArray[static_cast<unsigned char>(*it1)] == -1 || coefficientsArray[static_cast<unsigned char>(*(it1+1))] == -1 ) {
                continue;
            }

            if ( coefficientsArray[static_cast<unsigned char>(*it1)] == coefficientsArray[static_cast<unsigned char>(*(it1+1))] ) {
                continue;
            }
            
            // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
            if ( coefficientsArray[static_cast<unsigned char>(*(it1+1))] == coefficientsArray[static_cast<unsigned char>(*(it1+2))] ) {
                
                for ( it2 = it1 + 3; it2 < end && *(it2-1) == *(it2); it2++ );

                // If z is invalid character or end, then do not processed
                if ( it2 == end || coefficientsArray[static_cast<unsigned char>(*it2)] == -1 || coefficientsArray[static_cast<unsigned char>(*(it2-1))] == -1 ) {
                    continue;
                }

                // core constructor takes end exclusively
                it2++;
                            
                core *new_core = new core(it1, it2, index, index+(it2-it1), rev_comp);
                this->cores.push_back(new_core);
                
                continue;
            }

            // if there is a local minima
            if ( coefficientsArray[static_cast<unsigned char>(*(it1))] > coefficientsArray[static_cast<unsigned char>(*(it1+1))] && 
                 coefficientsArray[static_cast<unsigned char>(*(it1+1))] < coefficientsArray[static_cast<unsigned char>(*(it1+2))] ) {
                
                core *new_core = new core(it1, it1+3, index, index+3, rev_comp);
                this->cores.push_back(new_core);
                
                continue;
            } 
            
            // validate before checking further (corner cases for initial and last cores)
            if ( it1 == begin || it1 + 3 == end ) {
                continue;
            }

            // if there is local maxima without immediate local minima neighbours
            if ( coefficientsArray[static_cast<unsigned char>(*(it1+1))] > coefficientsArray[static_cast<unsigned char>(*(it1+2))] && 
                 coefficientsArray[static_cast<unsigned char>(*(it1))] < coefficientsArray[static_cast<unsigned char>(*(it1+1))] && 
                 coefficientsArray[static_cast<unsigned char>(*(it1-1))] <= coefficientsArray[static_cast<unsigned char>(*(it1))] && 
                 coefficientsArray[static_cast<unsigned char>(*(it1+2))] >= coefficientsArray[static_cast<unsigned char>(*(it1+3))] ) {
                
                core *new_core = new core(it1, it1+3, index, index+3, rev_comp);
                this->cores.push_back(new_core);
                
                continue;
            }
        }
    }

    lps::~lps() {
        for ( std::vector<core*>::iterator it = this->cores.begin(); it != this->cores.end(); it++ ) {
            delete *it;
        }
        this->cores.clear();
    }

    bool lps::deepen() {
        
        // Compress cores
        for( int compression_iteratin_index = 0; compression_iteratin_index < COMPRESSION_ITERATION_COUNT; compression_iteratin_index++ ) {
            
            // at least 2 cores are needed for compression
            if (this->cores.size() < 2) {
                for ( std::vector<core*>::iterator it = this->cores.begin(); it != this->cores.end(); it++ ) {
                    delete *it;
                }
                this->cores.clear();
                this->level++;
                return false;
            }

            std::vector<core*>::iterator it_curr = this->cores.end() - 1, it_left = this->cores.end() - 2;

            for( ; cores.begin() + start_index < it_curr; it_curr--, it_left-- ) {
                // std::cout << "compressing: " << *it_left << " " << *it_curr << " = "; 
                (*it_curr)->compress(*it_left);
                // std::cout << *it_curr << std::endl;
            }

            this->start_index++;
        }

        // find cores from compressed cores.
        std::vector<core*>::iterator it1 = this->cores.begin() + start_index, it2;
        std::vector<core*> temp_cores;

        temp_cores.reserve( this->cores.size() / CONSTANT_FACTOR );

        for ( ; it1 + 2 < this->cores.end(); it1++ ) {

            if ( *(*it1) == *(*(it1+1)) ) {
                continue;
            }

            // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
            if ( *(*(it1 + 1)) == *(*(it1+2)) ) {

                for ( it2 = it1 + 3; it2 < this->cores.end() && *(*(it2-1)) == *(*(it2)); it2++ );
                
                it2++;

                if ( it2 <= this->cores.end() ) {
                    core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it2);
                    temp_cores.push_back(new_core);
                } 

                continue;
            }

            // if there is a local minima
            if ( *(*(it1)) > *(*(it1+1)) && *(*(it1+1)) < *(*(it1+2)) ) {
                core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it1+3);
                temp_cores.push_back(new_core);
            }

            // validate before checking further (corner cases for initial and last cores)
            if ( it1 == this->cores.begin() || it1 + 3 == this->cores.end() ) {
                continue;
            }

            // if there is local maxima without immediate local minima neighbours


            if ( *(*(it1)) < *(*(it1 + 1)) && *(*(it1+1)) > *(*(it1+2)) && 
                 *(*(it1-1)) <= *(*(it1)) && *(*(it1+2)) >= *(*(it1+3)) ) { 
                
                core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it1+3);
                temp_cores.push_back(new_core);
            }
        }

        // remove old cores
        for ( std::vector<core*>::iterator it = this->cores.begin(); it != this->cores.end(); it++ ) {
            delete *it;
        }
        this->cores.clear();

        this->cores = temp_cores;
        this->start_index = 0;

        this->level++;
        
        return true;
    }

    bool lps::deepen(int lcp_level) {

        if ( lcp_level <= this->level ) {
            return false;
        }

        while ( this->level < lcp_level ) {
            this->deepen();
        }

        return true;
    }

    void lps::write(std::string filename) {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            std::cerr << "Error opening file for writing" << std::endl;
            return;
        }

        write(out);

        out.close();
    }

    void lps::write(std::ofstream& out) {
        out.write(reinterpret_cast<const char*>(&level), sizeof(level));
        out.write(reinterpret_cast<const char*>(&start_index), sizeof(start_index));
        size_t size = this->cores.size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // write each core object
        for (core* c : this->cores) {
            c->write(out);
        }
    }


    std::ostream& operator<<(std::ostream& os, const lps& element) {
        os << "Level: " << element.level << std::endl;
        for (core* c : element.cores) {
            os << c << " ";
        }
        return os;
    };

    std::ostream& operator<<(std::ostream& os, const lps* element) {
        os << "Level: " << element->level << std::endl;
        for (core* c : element->cores) {
            os << c << " ";
        }
        return os;
    };
};
