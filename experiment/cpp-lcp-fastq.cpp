#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           8


double mean(int distances[DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = 0;
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        m += i * distances[i];
        count += distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        m += larger_distances[i];
    }
    count += larger_distances.size();
    return m / count;
};


double std_deviation(int distances[DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = mean(distances, larger_distances);
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        count += distances[i];
    }
    count += larger_distances.size();
    double variance = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        variance += ( m - i ) * ( m - i ) * distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        variance += ( m - larger_distances[i] ) * ( m - larger_distances[i] );
    }
    return sqrt(variance / count);
};


double mean_shifted(int distances[2*DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = 0;
    double count = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        m += (i-DISTANCE_LENGTH) * distances[i];
        count += distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        m += larger_distances[i];
    }
    count += larger_distances.size();
    return m / count;
};


double std_deviation_shifted(int distances[2*DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = mean_shifted(distances, larger_distances);
    double count = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        count += distances[i];
    }
    count += larger_distances.size();
    double variance = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        variance += ( m - (i-DISTANCE_LENGTH) ) * ( m - (i-DISTANCE_LENGTH) ) * distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        variance += ( m - larger_distances[i] ) * ( m - larger_distances[i] );
    }
    return sqrt(variance / count);
};


void print2file(int all_distances[LCP_LEVEL][2*DISTANCE_LENGTH], int all_distances_pos[LCP_LEVEL][DISTANCE_LENGTH], int all_lengths[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, std::ofstream &outfile ) { 
    
    for ( int i = 0; i < LCP_LEVEL; i++ ) {
        
        outfile << "Level " << i << std::endl;

        outfile << "Distances ( core_curr->pos - core_prev->end ) " << std::endl;
        for ( int j = 0; j < 2*DISTANCE_LENGTH; j++ ) {
            for ( int k = 0; k < all_distances[i][j]; k++ ) {
                outfile << j - DISTANCE_LENGTH << ',';
            }
        }
        for ( int j = 0; j < all_larger_distances_vec[i].size(); j++ ) {
            outfile << all_larger_distances_vec[i][j] << ',';
        }
        outfile << std::endl;

        outfile << "Distances btw pos ( core_curr->pos - core_prev->pos ) " << std::endl;
        for ( int j = 0; j < DISTANCE_LENGTH; j++ ) {
            for ( int k = 0; k < all_distances_pos[i][j]; k++ ) {
                outfile << j << ',';
            }
        }
        for ( int j = 0; j < all_larger_distances_pos_vec[i].size(); j++ ) {
            outfile << all_larger_distances_pos_vec[i][j] << ',';
        }
        outfile << std::endl;

        outfile << "Lengths ( core_curr->end - core_curr->pos ) " << std::endl;
        for ( int j = 0; j < DISTANCE_LENGTH; j++ ) {
            for ( int k = 0; k < all_lengths[i][j]; k++ ) {
                outfile << j << ',';
            }
        }
        for ( int j = 0; j < all_larger_lengths_vec[i].size(); j++ ) {
            outfile << all_larger_lengths_vec[i][j] << ',';
        }

        outfile << std::endl << std::endl;
    }
};


void summary( int overlapping_counts[LCP_LEVEL], int all_distances[LCP_LEVEL][2*DISTANCE_LENGTH], int all_distances_pos[LCP_LEVEL][DISTANCE_LENGTH], int all_lengths[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, std::vector<std::chrono::milliseconds> &all_durations, int all_core_count[LCP_LEVEL]) {
    
    for ( int i = 0; i < LCP_LEVEL; i++ ) {

        std::cout << "Level: " << i << std::endl;
        std::cout << "Overlapping core counts:                  " << overlapping_counts[i] << std::endl;
        std::cout << "Std of distances btw cores (w'out):       " << std_deviation_shifted(all_distances[i]) << std::endl;
        std::cout << "Mean of distances btw cores (w'out):      " << mean_shifted(all_distances[i]) << std::endl;
        std::cout << "Std of distances btw cores (with):        " << std_deviation_shifted(all_distances[i], all_larger_distances_vec[i]) << std::endl;
        std::cout << "Mean of distances btw cores (with):       " << mean_shifted(all_distances[i], all_larger_distances_vec[i]) << std::endl;

        std::cout << "------------------------------------------" << std::endl;

        std::cout << "Std of distances btw starts (w'out):      " << std_deviation(all_distances_pos[i]) << std::endl;
        std::cout << "Mean of distances btw starts (w'out):     " << mean(all_distances_pos[i]) << std::endl;
        std::cout << "Std of distances btw starts (with):       " << std_deviation(all_distances_pos[i], all_larger_distances_pos_vec[i]) << std::endl;
        std::cout << "Mean of distances btw starts (with):      " << mean(all_distances_pos[i], all_larger_distances_pos_vec[i]) << std::endl;

        std::cout << "------------------------------------------" << std::endl;

        std::cout << "Std of lengths (w'out):                   " << std_deviation(all_lengths[i]) << std::endl;
        std::cout << "Mean of lengths (w'out):                  " << mean(all_lengths[i]) << std::endl;
        std::cout << "Std of lengths (with):                    " << std_deviation(all_lengths[i], all_larger_lengths_vec[i]) << std::endl;
        std::cout << "Mean of lengths (with):                   " << mean(all_lengths[i], all_larger_lengths_vec[i]) << std::endl;

        std::cout << "------------------------------------------" << std::endl;

        std::cout << "Level execution time:                     " << ( (double) all_durations[i].count() ) / 1000 << " ms" << std::endl;
        std::cout << "Total number of cores:                    " << all_core_count[i] << std::endl;
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "dist # not in [-10k,10k):                 " << all_larger_distances_vec.size() << std::endl;
    std::cout << "dist btw pos # not in [0,10k):            " << all_larger_distances_pos_vec.size() << std::endl; 
    std::cout << "length # [0,10k):                         " << all_larger_lengths_vec.size() << std::endl;
};


void analyze( int level, int overlapping_counts[LCP_LEVEL], int all_distances[LCP_LEVEL][2*DISTANCE_LENGTH], int all_distances_pos[LCP_LEVEL][DISTANCE_LENGTH], int all_lengths[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, lcp::string *str ) {

    for ( std::deque<lcp::core*>::iterator it = str->cores.begin() + 1; it < str->cores.end(); it++ ) {

        if ( (*it)->start < (*(it-1))->end ) {
            overlapping_counts[level] += 1;
        }
        if ( 0 <= (*it)->start - (*(it-1))->end + DISTANCE_LENGTH && (*it)->start - (*(it-1))->end < 2*DISTANCE_LENGTH ) {
            all_distances[level][(*it)->start - (*(it-1))->end + DISTANCE_LENGTH]++;
        } else {
            // outfile << "Invalid distance. curr->start: " << (*it)->start << " prev->end: " << (*(it-1))->end << " pos_distance: " << (*it)->start - (*(it-1))->end << std::endl;
            all_larger_distances_vec[level].push_back((*it)->start - (*(it-1))->end);
        }

        if ( 0 <= (*it)->start - (*(it-1))->start && (*it)->start - (*(it-1))->start < DISTANCE_LENGTH ) {
            all_distances_pos[level][(*it)->start - (*(it-1))->start]++;
        } else {
            // outfile << "Invalid pos distances. curr->start: " << (*it)->start << " prev->start: " << (*(it-1))->start << " pos_distance: " << (*it)->start - (*(it-1))->start << std::endl;
            all_larger_distances_pos_vec[level].push_back( (*it)->start - (*(it-1))->start );
        }
        
        if ( 0 <= (*it)->end - (*it)->start && (*it)->end - (*it)->start < DISTANCE_LENGTH ) {
            all_lengths[level][(*it)->end - (*it)->start] += 1;
        } else {
            // outfile << "Invalid length distances. curr->start: " << (*it)->start << " curr->end: " << (*it)->end << " length: " << (*it)->end - (*it)->start << std::endl;
            all_larger_lengths_vec[level].push_back( (*it)->end - (*it)->start );
        }
    }
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

    int read_count = 0;
    int all_overlapping_counts[LCP_LEVEL] = {0};
    int all_core_count[LCP_LEVEL] = {0};
    int all_distances[LCP_LEVEL][2*DISTANCE_LENGTH] = {0};  // in order to store overlapping cores as well (as distance will be negative)
    int all_distances_pos[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    int all_lengths[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    std::vector<std::chrono::milliseconds> all_durations(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_distances_vec(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_distances_pos_vec(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_lengths_vec(LCP_LEVEL);

	// read file
    if ( genome.is_open() ) {  

        std::chrono::milliseconds total_duration;
        
        std::cout << "Program begins" << std::endl;

        while ( true ) {

            if (getline(genome, line)) {

                read_count++;

                // line is id

                getline(genome, line); // raw sequence

                auto start = std::chrono::high_resolution_clock::now();
                
                lcp::string *str = new lcp::string(line);
                
                auto extraction_end = std::chrono::high_resolution_clock::now();
                all_durations[0] += std::chrono::duration_cast<std::chrono::milliseconds>(extraction_end - start);
                all_core_count[0] += str->cores.size();

                analyze(0, all_overlapping_counts, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, str);

                for ( int i = 1; i < LCP_LEVEL; i++ ) {

                    auto start_level = std::chrono::high_resolution_clock::now();

                    str->deepen();
                    
                    auto stop_level = std::chrono::high_resolution_clock::now();
                    auto duration_level = std::chrono::duration_cast<std::chrono::milliseconds>(stop_level - start_level);
                    all_durations[i] += std::chrono::milliseconds(duration_level.count());
                    all_core_count[i] += str->cores.size();

                    analyze(i, all_overlapping_counts, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, str);
                }

                delete str;

                getline(genome, line); // +
                
                getline(genome, line); // quality
            
            } else {
                break;
            }
        }
        
        summary( all_overlapping_counts, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, all_durations, all_core_count);
        
        genome.close();
    }

    std::ofstream outfile ( "detailed_summary.fastq.txt" );
    
    print2file( all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, outfile );

    outfile.close();

    return 0;
};
