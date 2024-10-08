#include "lps.h"

namespace lcp {

    lps::lps(std::string &str, bool use_map, bool rev_comp) {

        this->level = 1;

        std::string::iterator it2;
        
        this->cores = new std::vector<lcp::core*>;
        this->cores->reserve( str.size() / CONSTANT_FACTOR );       // As each core appears with average distance of 2.27,
                                                                    // and the increase is always above the 2, it makes sense to reserve
                                                                    // half of the size for the cores to prevent expansion of vector.
        
        if ( rev_comp ) {
            std::reverse(str.begin(), str.end());
        }
        
        parse(str.begin(), str.end(), use_map, rev_comp);
    };

    lps::lps(std::ifstream& in) {
        in.read(reinterpret_cast<char*>(&level), sizeof(level));
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));

        // read each core object
        
        // resize the vector to the appropriate size
        this->cores = new std::vector<lcp::core*>;
        this->cores->reserve(size);

        // read each core object
        for (size_t i = 0; i < size; i++) {
            core* new_core = new core(in);
            this->cores->push_back(new_core);
        }
    };

    void lps::parse(std::string::iterator begin, std::string::iterator end, bool use_map, bool rev_comp ) {

        int* coefficientsArray = ( rev_comp ? reverse_complement_coefficients : coefficients);

        int index = 0;
        size_t last_index = end - begin;
        std::string::iterator it1 = begin;
        std::string::iterator it2 = end;

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
                            
                if ( it2 < it1 ) {
                    core *new_core = new core(it2-1, it1+1, last_index, rev_comp);
                    this->cores->push_back(new_core);

                    if ( use_map ) {
                        new_core->label = hash::emplace( it2-1, it1+1 );
                    } else {
                        new_core->label = hash::simple( it2-1, it1+1 );
                    }
                }

                for ( it2 = it1 + 3; it2 < end && *(it2-1) == *(it2); it2++ );

                // If z is invalid character or end, then do not processed
                if ( it2 == end || coefficientsArray[static_cast<unsigned char>(*it2)] == -1 || coefficientsArray[static_cast<unsigned char>(*(it2-1))] == -1 ) {
                    continue;
                }

                // core constructor takes end exclusively
                it2++;
                            
                core *new_core = new core(it1, it2, index, rev_comp);
                this->cores->push_back(new_core);
                
                // set label
                if ( use_map ) {
                    new_core->label = hash::emplace( it1, it2 );
                } else {
                    new_core->label = hash::simple( it1, it2 );
                }

                last_index = index+(it2-it1)-1;
                
                continue;
            }

            // if there is a local minima
            if ( coefficientsArray[static_cast<unsigned char>(*(it1))] > coefficientsArray[static_cast<unsigned char>(*(it1+1))] && 
                 coefficientsArray[static_cast<unsigned char>(*(it1+1))] < coefficientsArray[static_cast<unsigned char>(*(it1+2))] ) {
                
                if ( it2 < it1 ) {
                    core *new_core = new core(it2-1, it1+1, last_index, rev_comp);
                    this->cores->push_back(new_core);

                    if ( use_map ) {
                        new_core->label = hash::emplace( it2-1, it1+1 );
                    } else {
                        new_core->label = hash::simple( it2-1, it1+1 );
                    }
                }

                it2 = it1 + 3;
                
                core *new_core = new core(it1, it2, index, rev_comp);
                this->cores->push_back(new_core);
                
                // set label
                if ( use_map ) {
                    new_core->label = hash::emplace( it1, it2 );
                } else {
                    new_core->label = hash::simple( it1, it2 );
                }

                last_index = index + 2;

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
                
                if ( it2 < it1 ) {
                    core *new_core = new core(it2-1, it1+1, last_index, rev_comp);
                    this->cores->push_back(new_core);

                    if ( use_map ) {
                        new_core->label = hash::emplace( it2-1, it1+1 );
                    } else {
                        new_core->label = hash::simple( it2-1, it1+1 );
                    }
                }

                it2 = it1 + 3;

                core *new_core = new core(it1, it2, index, rev_comp);
                this->cores->push_back(new_core);
                
                // set label
                if ( use_map ) {
                    new_core->label = hash::emplace( it1, it2 );
                } else {
                    new_core->label = hash::simple( it1, it2 );
                }

                last_index = index + 2;

                continue;
            }
        }
    };

    lps::~lps() {
        for ( std::vector<core*>::iterator it = this->cores->begin(); it != this->cores->end(); it++ ) {
            delete *it;
        }
        delete this->cores;
    };

    bool lps::deepen( bool use_map ) {

        size_t start_index = 0;

        // Compress cores
        for( int compression_iteratin_index = 0; compression_iteratin_index < COMPRESSION_ITERATION_COUNT; compression_iteratin_index++ ) {
            
            // at least 2 cores are needed for compression
            if (this->cores->size() < 2) {
                for ( std::vector<core*>::iterator it = this->cores->begin(); it != this->cores->end(); it++ ) {
                    delete *it;
                }
                this->cores->clear();
                this->level++;
                return false;
            }

            std::vector<core*>::iterator it_curr = this->cores->end() - 1, it_left = this->cores->end() - 2;

            for( ; this->cores->begin() + start_index < it_curr; it_curr--, it_left-- ) {
                (*it_curr)->compress(*it_left);
            }

            start_index++;
        }

        // find cores from compressed cores.
        std::vector<core*>::iterator it1 = this->cores->begin() + start_index, it2 = this->cores->end();
        std::vector<core*> *temp_cores = new std::vector<core*>;

        temp_cores->reserve( this->cores->size() / CONSTANT_FACTOR );

        for ( ; it1 + 2 < this->cores->end(); it1++ ) {

            if ( *(*it1) == *(*(it1+1)) ) {
                continue;
            }

            // if there are same characters in subsequenct order such as xyyz, xyyyz, .... where x!=y and y!=z
            if ( *(*(it1 + 1)) == *(*(it1+2)) ) {
                
                // check if there is a gap in between previous core
                if ( it2 < it1 ) {
                    core *new_core = new core(it2-1-COMPRESSION_ITERATION_COUNT, it1+1);
                    temp_cores->push_back(new_core);

                    if ( use_map ) {
                        new_core->label = hash::emplace( (*(it2-1))->label, (*(it2))->label, (*(it1))->label, static_cast<uint32_t>(it1-it2) );
                    } else {
                        new_core->label = hash::simple( (*(it2-1))->label, (*(it2))->label, (*(it1))->label, static_cast<uint32_t>(it1-it2) );
                    }
                }

                for ( it2 = it1 + 3; it2 < this->cores->end() && *(*(it2-1)) == *(*(it2)); it2++ );
                
                it2++;

                if ( it2 <= this->cores->end() ) {
                    core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it2);
                    temp_cores->push_back(new_core);

                    // set label
                    if ( use_map ) {
                        new_core->label = hash::emplace( (*(it1))->label, (*(it1+1))->label, (*(it2-1))->label, static_cast<uint32_t>(it1-it2) );
                    } else {
                        new_core->label = hash::simple( (*(it1))->label, (*(it1+1))->label, (*(it2-1))->label, static_cast<uint32_t>(it1-it2) );
                    }
                } 

                continue;
            }

            // if there is a local minima
            if ( *(*(it1)) > *(*(it1+1)) && *(*(it1+1)) < *(*(it1+2)) ) {
                
                // check if there is a gap in between previous core
                if ( it2 < it1 ) {
                    core *new_core = new core(it2-1-COMPRESSION_ITERATION_COUNT, it1+1);
                    temp_cores->push_back(new_core);

                    if ( use_map ) {
                        new_core->label = hash::emplace( (*(it2-1))->label, (*(it2))->label, (*(it1))->label, static_cast<uint32_t>(it1-it2) );
                    } else {
                        new_core->label = hash::simple( (*(it2-1))->label, (*(it2))->label, (*(it1))->label, static_cast<uint32_t>(it1-it2) );
                    }
                }

                it2 = it1 + 3;

                core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it2);
                temp_cores->push_back(new_core);

                // set label
                if ( use_map ) {
                    new_core->label = hash::emplace( (*(it1))->label, (*(it1+1))->label, (*(it1+2))->label, 1 );
                } else {
                    new_core->label = hash::simple( (*(it1))->label, (*(it1+1))->label, (*(it1+2))->label, 1 );
                }

                continue;
            }

            // validate before checking further (corner cases for initial and last cores)
            if ( it1 == this->cores->begin() || it1 + 3 == this->cores->end() ) {
                continue;
            }

            // if there is local maxima without immediate local minima neighbours
            if ( *(*(it1)) < *(*(it1 + 1)) && *(*(it1+1)) > *(*(it1+2)) && 
                 *(*(it1-1)) <= *(*(it1)) && *(*(it1+2)) >= *(*(it1+3)) ) { 
                
                // check if there is a gap in between previous core
                if ( it2 < it1 ) {
                    core *new_core = new core(it2-1-COMPRESSION_ITERATION_COUNT, it1+1);
                    temp_cores->push_back(new_core);

                    if ( use_map ) {
                        new_core->label = hash::emplace( (*(it2-1))->label, (*(it2))->label, (*(it1))->label, static_cast<uint32_t>(it1-it2) );
                    } else {
                        new_core->label = hash::simple( (*(it2-1))->label, (*(it2))->label, (*(it1))->label, static_cast<uint32_t>(it1-it2) );
                    }
                }

                it2 = it1 + 3;

                core *new_core = new core(it1 - COMPRESSION_ITERATION_COUNT, it2);
                temp_cores->push_back(new_core);

                // set label
                if ( use_map ) {
                    new_core->label = hash::emplace( (*(it1))->label, (*(it1+1))->label, (*(it1+2))->label, 1 );
                } else {
                    new_core->label = hash::simple( (*(it1))->label, (*(it1+1))->label, (*(it1+2))->label, 1 );
                }

                continue;
            }
        }

        // remove old cores
        for ( std::vector<core*>::iterator it = this->cores->begin(); it != this->cores->end(); it++ ) {
            delete *it;
        }
        delete this->cores;

        this->cores = temp_cores;
        temp_cores = nullptr;

        this->level++;
        
        return true;
    };

    bool lps::deepen( int lcp_level, bool use_map ) {

        if ( lcp_level <= this->level ) {
            return false;
        }

        while ( this->level < lcp_level ) {
            this->deepen(use_map);
        }

        return true;
    };

    void lps::write(std::string filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) {
            std::cerr << "Error opening file for writing" << std::endl;
            return;
        }

        write(out);

        out.close();
    };

    void lps::write(std::ofstream& out) const {
        out.write(reinterpret_cast<const char*>(&level), sizeof(level));
        size_t size = this->cores->size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // write each core object
        for ( std::vector<core*>::iterator it = this->cores->begin(); it != this->cores->end(); it++ ) {
            (*it)->write(out);
        }
    };

    double lps::memsize() {
        double total = sizeof(*this);
        total += this->cores->capacity() * sizeof(core*);

        for ( std::vector<core*>::iterator it = this->cores->begin(); it != this->cores->end(); it++ ) {
            total += (*it)->memsize();
        }

        return total;
    };

    bool lps::set_core_count( std::vector<uint32_t>& core_count ) {

        if ( lcp::hash::reverse_map.size() == 0 ) {
            return false;
        }
        
        for ( std::vector<lcp::core*>::iterator it = this->cores->begin(); it < this->cores->end(); it++ ) {
            lcp::hash::count_core( core_count, (*it)->label );
        }
        
        return true;
    };

    bool lps::update_core_count( std::vector<uint32_t>& core_count ) {
        for ( std::vector<lcp::core*>::iterator it = this->cores->begin(); it < this->cores->end(); it++ ) {
            core_count[(*it)->label]++;
        }

        return true;
    };
    
    bool lps::get_labels(std::vector<uint32_t>& labels) {

        labels.reserve( labels.size() + this->cores->size() );

        for ( std::vector<lcp::core*>::iterator it = this->cores->begin(); it < this->cores->end(); it++ ) {
            labels.push_back( (*it)->label );
        }
        
        return true;
    };

    std::ostream& operator<<(std::ostream& os, const lps& element) {
        os << "Level: " << element.level << std::endl;
        for ( std::vector<core*>::iterator it = element.cores->begin(); it != element.cores->end(); it++ ) {
            os << (*it) << " ";
        }
        return os;
    };

    std::ostream& operator<<(std::ostream& os, const lps* element) {
        os << "Level: " << element->level << std::endl;
        for ( std::vector<core*>::iterator it = element->cores->begin(); it != element->cores->end(); it++ ) {
            os << (*it) << " ";
        }
        return os;
    };
};
