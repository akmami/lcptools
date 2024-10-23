#include "lps.h"


namespace lcp {
    
    lps::lps(std::string &str, const int lcp_level, const size_t sequence_split_length, const size_t overlap_margin_length) {
        
        this->level = 1;
        
        // create initial cores based on first sequence split
        this->cores = new std::vector<lcp::core*>;
        this->cores->reserve( sequence_split_length / CONSTANT_FACTOR );

        std::string::iterator it = str.begin();
        parse(it, std::min(it + sequence_split_length, str.end()));
        this->deepen(lcp_level);
        
        // reserve the size based on the estimated core size for whole string
        this->cores->reserve( this->cores->size() * ( str.size() / sequence_split_length + 1 ));

        // move to next split
        it += sequence_split_length;

        while( it < str.end() ) {

            std::string::iterator begin = it - overlap_margin_length;
            std::string::iterator end = std::min(it + sequence_split_length, str.end()); 
            std::string str(begin, end);   
            lcp::lps* temp = new lcp::lps(str);
            temp->deepen(lcp_level);
            
            // merge new processed segment with main cores
            size_t max_overlap_size = 4;
            size_t overlap_index = max_overlap_size;
            size_t max_overlap_index = 50 < temp->cores->size() ? 50 : temp->cores->size();
            bool found = false; // index for overlap

            while ( overlap_index <= max_overlap_index ) {

                size_t match_count = 0;
                std::vector<lcp::core*>::iterator it1, it2;
                
                it1 = this->cores->end() - 1;
                it2 = temp->cores->begin() + overlap_index - 1;

                while ( match_count < max_overlap_size && *(*(it1 - match_count)) == *(*(it2 - match_count)) ) {
                    match_count++;
                }
                
                // found overlap with size overlap_size
                if ( match_count == max_overlap_size ) {
                    found = true;
                    break;
                }

                // try larger overlap
                overlap_index++;
            }

            if ( !found ) {
                overlap_index = 0;
            }

            // append new cores to the original core vector
            this->cores->insert(this->cores->end(), temp->cores->begin()+overlap_index, temp->cores->end());

            // remove non-overlapping cores so it wont be deleted
            temp->cores->erase(temp->cores->begin()+overlap_index, temp->cores->end());
            delete temp;

            // move next segment
            it += sequence_split_length;
        }
    };

    lps::lps(std::string::iterator begin, std::string::iterator end) {
        this->level = 1;
        
        this->cores = new std::vector<lcp::core*>;
        this->cores->reserve( (end - begin) / CONSTANT_FACTOR );

        parse(begin, end);
    };

    lps::lps(std::string &str, bool use_map, bool rev_comp) {

        this->level = 1;

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

    bool operator==(const lcp::lps& lhs, const lcp::lps& rhs) {
        if ( lhs.cores->size() != rhs.cores->size() ) {
            return false;
        }

        for ( std::vector<lcp::core*>::const_iterator lit = lhs.cores->begin(), rit = rhs.cores->begin(); lit <  lhs.cores->end(); lit++, rit++ ) {
            if ( *(*lit) != *(*rit) ) {
                
                return false;
            }
        }

        return true;
    };

    bool operator!=(const lcp::lps& lhs, const lcp::lps& rhs) {
        if ( lhs.cores->size() != rhs.cores->size() ) {
            return true;
        }

        for ( std::vector<lcp::core*>::const_iterator lit = lhs.cores->begin(), rit = rhs.cores->begin(); lit <  lhs.cores->end(); lit++, rit++ ) {
            if ( *(*lit) != *(*rit) ) {
                return true;
            }
        }

        return false;
    };
};
