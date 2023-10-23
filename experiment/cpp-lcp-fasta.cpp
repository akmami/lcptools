#include <iostream>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000

struct overflow {
    int larger_distances = 0;
    int larger_pos_distances = 0;
    int larger_lengths = 0;
};


double mean(int distances[DISTANCE_LENGTH]) {
    double m = 0;
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        m += i * distances[i];
        count += distances[i];
    }
    return m / count;
};


double std_deviation(int distances[DISTANCE_LENGTH]) {
    double m = mean(distances);
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        count += distances[i];
    }
    double variance = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        variance += ( m - i ) * ( m - i ) * distances[i];
    }
    return sqrt(variance / count);
};


double mean_shifted(int distances[2*DISTANCE_LENGTH]) {
    double m = 0;
    double count = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        m += (i-DISTANCE_LENGTH) * distances[i];
        count += distances[i];
    }
    return m / count;
};


double std_deviation_shifted(int distances[2*DISTANCE_LENGTH]) {
    double m = mean_shifted(distances);
    double count = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        count += distances[i];
    }
    double variance = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        variance += ( m - (i-DISTANCE_LENGTH) ) * ( m - (i-DISTANCE_LENGTH) ) * distances[i];
    }
    return sqrt(variance / count);
};


void summary( int level, overflow &of, lcp::string *str, std::chrono::seconds duration) {
    int overlapping_count = 0;

    int distances[2*DISTANCE_LENGTH] = {0};  // in order to store overlapping cores as well (as distance will be negative)
    int distances_pos[DISTANCE_LENGTH] = {0};
    int lengths[DISTANCE_LENGTH] = {0};

    for ( std::deque<lcp::core*>::iterator it = str->cores.begin() + 1; it < str->cores.end(); it++ ) {
        // std::cout << "Processing new core " << (*it)->start << " " << (*it)->end << std::endl;
        // std::cout << *it << std::endl;
        // std::cout << (*it)->start << " " << (*(it-1))->end << std::endl;
        // std::cout << (*it)->start << " " << (*(it-1))->start << std::endl;
        // std::cout << (*it)->end << " " << (*it)->start << std::endl;
        if ( (*it)->start < (*(it-1))->end ) {
            overlapping_count += 1;
        }
        if ( (*it)->start - (*(it-1))->end < DISTANCE_LENGTH ) {
            if ((*it)->start - (*(it-1))->end + DISTANCE_LENGTH < 0 || (*it)->start - (*(it-1))->end + DISTANCE_LENGTH >= 2*DISTANCE_LENGTH) {
                std::cout << "Something wrong" << std::endl;
                std::cout << (*it)->start << " " << (*(it-1))->end << std::endl;
                std::cout << (*it)->start << " " << (*(it-1))->start << std::endl;
                std::cout << (*it)->end << " " << (*it)->start << std::endl;
            }
            distances[(*it)->start - (*(it-1))->end + DISTANCE_LENGTH]++;
        } else {
            of.larger_distances++;
        }

        if ( (*it)->start - (*(it-1))->start < DISTANCE_LENGTH ) {
            if ((*it)->start - (*(it-1))->start < 0 || (*it)->start - (*(it-1))->start >= DISTANCE_LENGTH) {
                std::cout << "Something wrong" << std::endl;
                std::cout << (*it)->start << " " << (*(it-1))->end << std::endl;
                std::cout << (*it)->start << " " << (*(it-1))->start << std::endl;
                std::cout << (*it)->end << " " << (*it)->start << std::endl;
            }
            distances_pos[(*it)->start - (*(it-1))->start]++;
        } else {
            of.larger_pos_distances++;
        }
        
        if ( (*it)->end - (*it)->start < DISTANCE_LENGTH ) {
            if ((*it)->end - (*it)->start < 0 || (*it)->end - (*it)->start >= DISTANCE_LENGTH) {
                std::cout << "Something wrong" << std::endl;
                std::cout << (*it)->start << " " << (*(it-1))->end << std::endl;
                std::cout << (*it)->start << " " << (*(it-1))->start << std::endl;
                std::cout << (*it)->end << " " << (*it)->start << std::endl;
            }
            lengths[(*it)->end - (*it)->start] += 1;
        } else {
            of.larger_lengths++;
        }
    }

    std::cout << "Level: " << level << std::endl;
    std::cout << "Overlapping core counts:          " << overlapping_count << std::endl;
    std::cout << "Std of distances btw cores:       " << std_deviation_shifted(distances) << std::endl;
    std::cout << "Mean of distances btw cores:      " << mean_shifted(distances) << std::endl;
    std::cout << "Std of distances btw starts:      " << std_deviation(distances_pos) << std::endl;
    std::cout << "Mean of distances btw starts:     " << mean(distances_pos) << std::endl;
    std::cout << "Std of lengths:                   " << std_deviation(lengths) << std::endl;
    std::cout << "Mean of lengths:                  " << mean(lengths) << std::endl;             
    std::cout << "Level execution time:             " << duration.count() << " sec" << std::endl;
    std::cout << "Total number of cores:            " << str->cores.size() << std::endl;
    std::cout << std::endl;
};


int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] " << std::endl;
        return -1;  
    }

    std::ifstream input(argv[1]);
    if (!input.good()) {
        std::cerr << "Error opening: " << argv[1] << " . You have failed." << std::endl;
        return -1;
    }

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string line;

    std::fstream genome;
    genome.open(argv[1], std::ios::in);

	// read file
    if ( genome.is_open() ) {  

        std::string sequence, id;
        sequence.reserve(250000000);
        std::chrono::seconds total_duration;
        overflow of;
        
        std::cout << "Program begins" << std::endl;

        while (getline(genome, line)) {

            if (line[0] == '>') {

                if (sequence.size() == 0) {
                    id = line.substr(1);
                    std::cout << "Processing started for " << id << std::endl;
                    continue;
                }
                
                auto start = std::chrono::high_resolution_clock::now();
                
                lcp::string *str = new lcp::string(sequence);
                
                auto extraction_end = std::chrono::high_resolution_clock::now();
                auto duration_processing = std::chrono::duration_cast<std::chrono::seconds>(extraction_end - start);

                summary(0, of, str, duration_processing);
                
                if (str->cores.size() > 2) {

                    for ( int i = 1; i <= 7; i++ ) {

                        auto start_level = std::chrono::high_resolution_clock::now();

                        str->deepen();
                        
                        auto stop_level = std::chrono::high_resolution_clock::now();
                        auto duration_level = std::chrono::duration_cast<std::chrono::seconds>(stop_level - start_level);
                        duration_processing += duration_level;

                        summary(i, of, str, duration_level);
                    }

                    std::cout << "Processing is done for " << id << std::endl;
                    std::cout << "Length of the processed sequence: " << sequence.size() << std::endl;
                    std::cout << "Total execution time:             " << duration_processing.count() << " sec" << std::endl;
                }

                total_duration += duration_processing;
                sequence.clear();
            }
            else if (line[0] != '>'){
                sequence += line;
            }
        }

        std::cout << "Core count with len >10k:         " << of.larger_lengths << std::endl;
        std::cout << "Core count with pos_dist >10k:    " << of.larger_pos_distances << std::endl;
        std::cout << "Core count with dist >10k:        " << of.larger_distances << std::endl;
        
        genome.close();
        
        std::cout << "Total execution time of the program:  " << total_duration.count() << std::endl;
    }

    return 0;
};
