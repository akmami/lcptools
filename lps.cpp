#include "lps.h"


namespace lcp {
    
    lps::lps( std::string &str, const int lcp_level, const size_t sequence_split_length, const size_t overlap_margin_length ) {
        
        this->level = 1;
        
        // create initial cores based on first sequence split
        this->cores = new std::vector<struct core>;
        this->cores->reserve( sequence_split_length / CONSTANT_FACTOR );

        std::string::iterator it = str.begin();

        parse(it, std::min(it + sequence_split_length, str.end()), this->cores, 0, char_gt, char_lt, char_eq, false);

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
                std::vector<struct core>::iterator it1, it2;
                
                it1 = this->cores->end() - 1;
                it2 = temp->cores->begin() + overlap_index - 1;

                while ( match_count < max_overlap_size && *((it1 - match_count)) == *((it2 - match_count)) ) {
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

    lps::lps( std::string::iterator begin, std::string::iterator end ) {
        
        this->level = 1;
        
        this->cores = new std::vector<struct core>;
        this->cores->reserve( (end - begin) / CONSTANT_FACTOR );

        parse(begin, end, this->cores, 0, char_gt, char_lt, char_eq, false);
    };

    lps::lps( std::string &str, bool use_map, bool rev_comp ) {

        this->level = 1;

        this->cores = new std::vector<struct core>;
        this->cores->reserve( str.size() / CONSTANT_FACTOR );
        
        if ( rev_comp ) {
            std::reverse(str.begin(), str.end());
        }
        
        parse(str.begin(), str.end(), this->cores, 0, char_gt, char_lt, char_eq, use_map);
    };

    lps::lps( std::ifstream& in ) {
        in.read(reinterpret_cast<char*>(&level), sizeof(level));
        size_t size;
        in.read(reinterpret_cast<char*>(&size), sizeof(size));

        // read each core object
        
        // resize the vector to the appropriate size
        this->cores = new std::vector<struct core>;
        this->cores->reserve(size);

        // read each core object
        for (size_t i = 0; i < size; i++) {
            this->cores->emplace_back(in);
        }
    };

    lps::~lps() {
        if ( this->cores != nullptr )
            delete this->cores;
        this->cores = nullptr;
    };

    bool lps::dct() {

        // at least 2 cores are needed for compression
        if ( this->cores == nullptr || this->cores->size() < COMPRESSION_ITERATION_COUNT + 2 ) {
            return false;
        }

        for( size_t compression_number = 0; compression_number < COMPRESSION_ITERATION_COUNT; compression_number++ ) {

            std::vector<struct core>::iterator it_curr = this->cores->end() - 1, it_left = this->cores->end() - 2;

            for( ; this->cores->begin() + compression_number < it_left; it_curr--, it_left-- ) {
                (it_curr)->compress(*it_left);
            }
        }

        return true;
    };

    bool lps::deepen( bool use_map ) {

        // Compress cores
        if ( ! dct() ) {
            if ( this->cores != nullptr )
                delete this->cores;
            this->cores = nullptr;
            return false;
        }

        // Find new cores
        std::vector<struct core> *temp_cores = new std::vector<struct core>;
        temp_cores->reserve( this->cores->size() / CONSTANT_FACTOR );
        
        parse( this->cores->begin() + COMPRESSION_ITERATION_COUNT, this->cores->end(), temp_cores, COMPRESSION_ITERATION_COUNT, core_gt, core_lt, core_eq, use_map);

        // remove old cores
        delete this->cores;

        this->cores = temp_cores;
        temp_cores = nullptr;

        this->level++;

        return true;
    };

    bool lps::deepen( int lcp_level, bool use_map ) {

        while ( this->level < lcp_level && this->deepen( use_map ) );

        return true;
    };

    void lps::write( std::ofstream& out ) const {
        out.write(reinterpret_cast<const char*>(&level), sizeof(level));
        size_t size = this->cores == nullptr ? 0 : this->cores->size();
        out.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // write each core object
        if ( this->cores != nullptr ) {
            for ( std::vector<struct core>::iterator it = this->cores->begin(); it != this->cores->end(); it++ ) {
                (it)->write(out);
            }
        }
    };

    double lps::memsize() const {
        double total = sizeof(*this);
        total += ( this->cores->capacity() - this->cores->size() ) * sizeof(struct core);
        
        if ( this->cores != nullptr ) {
            for ( std::vector<struct core>::iterator it = this->cores->begin(); it != this->cores->end(); it++ ) {
                total += (it)->memsize();
            }
        }

        return total;
    };
    
    bool lps::get_labels( std::vector<uint32_t>& labels ) const {

        if ( this->cores != nullptr ) {
            labels.reserve( labels.size() + this->cores->size() );

            for ( std::vector<struct core>::iterator it = this->cores->begin(); it < this->cores->end(); it++ ) {
                labels.push_back( (it)->label );
            }
        }
        
        return true;
    };

    size_t lps::size() const {
        return this->cores->size();
    };

    std::ostream& operator << ( std::ostream& os, const lps& element ) {
        os << "Level: " << element.level << std::endl;
        if ( element.cores != nullptr ) {
            for ( std::vector<struct core>::iterator it = element.cores->begin(); it != element.cores->end(); it++ ) {
                os << (*it) << " ";
            }
        }
        return os;
    };

    std::ostream& operator << ( std::ostream& os, const lps* element ) {
        os << "Level: " << element->level << std::endl;
        if ( element->cores != nullptr ) {
            for ( std::vector<struct core>::iterator it = element->cores->begin(); it != element->cores->end(); it++ ) {
                os << (*it) << " ";
            }
        }
        return os;
    };

    bool operator == ( const lcp::lps& lhs, const lcp::lps& rhs ) {
        if ( lhs.cores->size() != rhs.cores->size() ) {
            return false;
        }

        for ( std::vector<struct core>::const_iterator lit = lhs.cores->begin(), rit = rhs.cores->begin(); lit <  lhs.cores->end(); lit++, rit++ ) {
            if ( *(lit) != *(rit) ) {
                
                return false;
            }
        }

        return true;
    };

    bool operator != ( const lcp::lps& lhs, const lcp::lps& rhs ) {
        if ( lhs.cores->size() != rhs.cores->size() ) {
            return true;
        }

        for ( std::vector<struct core>::const_iterator lit = lhs.cores->begin(), rit = rhs.cores->begin(); lit <  lhs.cores->end(); lit++, rit++ ) {
            if ( *(lit) != *(rit) ) {
                return true;
            }
        }

        return false;
    };
};
