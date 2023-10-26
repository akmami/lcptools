#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           8

struct overflow {
    int larger_distances = 0;
    int larger_pos_distances = 0;
    int larger_lengths = 0;
};


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


void summary( int level, int all_distances[LCP_LEVEL][2*DISTANCE_LENGTH], int all_distances_pos[LCP_LEVEL][DISTANCE_LENGTH], int all_lengths[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, overflow &of, lcp::string *str, std::chrono::seconds duration) {
    
    int overlapping_count = 0;

    int distances[2*DISTANCE_LENGTH] = {0};  // in order to store overlapping cores as well (as distance will be negative)
    int distances_pos[DISTANCE_LENGTH] = {0};
    int lengths[DISTANCE_LENGTH] = {0};
    
    std::vector<int> larger_distances_vec;
    std::vector<int> larger_distances_pos_vec;
    std::vector<int> larger_lengths_vec;


    for ( std::deque<lcp::core*>::iterator it = str->cores.begin() + 1; it < str->cores.end(); it++ ) {

        if ( (*it)->start < (*(it-1))->end ) {
            overlapping_count += 1;
        }
        if ( 0 <= (*it)->start - (*(it-1))->end + DISTANCE_LENGTH && (*it)->start - (*(it-1))->end < 2*DISTANCE_LENGTH ) {
            distances[(*it)->start - (*(it-1))->end + DISTANCE_LENGTH]++;
            all_distances[level][(*it)->start - (*(it-1))->end + DISTANCE_LENGTH]++;
        } else {
            // outfile << "Invalid distance. curr->start: " << (*it)->start << " prev->end: " << (*(it-1))->end << " pos_distance: " << (*it)->start - (*(it-1))->end << std::endl;
            larger_distances_vec.push_back((*it)->start - (*(it-1))->end);
            all_larger_distances_vec[level].push_back((*it)->start - (*(it-1))->end);
        }

        if ( 0 <= (*it)->start - (*(it-1))->start && (*it)->start - (*(it-1))->start < DISTANCE_LENGTH ) {
            distances_pos[(*it)->start - (*(it-1))->start]++;
            all_distances_pos[level][(*it)->start - (*(it-1))->start]++;
        } else {
            // outfile << "Invalid pos distances. curr->start: " << (*it)->start << " prev->start: " << (*(it-1))->start << " pos_distance: " << (*it)->start - (*(it-1))->start << std::endl;
            larger_distances_pos_vec.push_back( (*it)->start - (*(it-1))->start );
            all_larger_distances_pos_vec[level].push_back( (*it)->start - (*(it-1))->start );
        }
        
        if ( 0 <= (*it)->end - (*it)->start && (*it)->end - (*it)->start < DISTANCE_LENGTH ) {
            lengths[(*it)->end - (*it)->start] += 1;
            all_lengths[level][(*it)->end - (*it)->start] += 1;
        } else {
            // outfile << "Invalid length distances. curr->start: " << (*it)->start << " curr->end: " << (*it)->end << " length: " << (*it)->end - (*it)->start << std::endl;
            larger_lengths_vec.push_back( (*it)->end - (*it)->start );
            all_larger_lengths_vec[level].push_back( (*it)->end - (*it)->start );
        }
    }

    of.larger_distances += larger_distances_vec.size();
    of.larger_pos_distances += larger_distances_pos_vec.size();
    of.larger_lengths += larger_lengths_vec.size();

    std::cout << "Level: " << level << std::endl;
    std::cout << "Overlapping core counts:                  " << overlapping_count << std::endl;
    std::cout << "Std of distances btw cores (w'out):       " << std_deviation_shifted(distances) << std::endl;
    std::cout << "Mean of distances btw cores (w'out):      " << mean_shifted(distances) << std::endl;
    std::cout << "Std of distances btw cores (with):        " << std_deviation_shifted(distances, larger_distances_vec) << std::endl;
    std::cout << "Mean of distances btw cores (with):       " << mean_shifted(distances, larger_distances_vec) << std::endl;
    std::cout << "dist # not in [-10k,10k):                 " << larger_distances_vec.size() << std::endl;

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "Std of distances btw starts (w'out):      " << std_deviation(distances_pos) << std::endl;
    std::cout << "Mean of distances btw starts (w'out):     " << mean(distances_pos) << std::endl;
    std::cout << "Std of distances btw starts (with):       " << std_deviation(distances_pos, larger_distances_pos_vec) << std::endl;
    std::cout << "Mean of distances btw starts (with):      " << mean(distances_pos, larger_distances_pos_vec) << std::endl;
    std::cout << "dist btw pos # not in [0,10k):            " << larger_distances_pos_vec.size() << std::endl; 

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "Std of lengths (w'out):                   " << std_deviation(lengths) << std::endl;
    std::cout << "Mean of lengths (w'out):                  " << mean(lengths) << std::endl;
    std::cout << "Std of lengths (with):                    " << std_deviation(lengths, larger_lengths_vec) << std::endl;
    std::cout << "Mean of lengths (with):                   " << mean(lengths, larger_lengths_vec) << std::endl;
    std::cout << "length # [0,10k):                         " << larger_lengths_vec.size() << std::endl;

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "Level execution time:                     " << duration.count() << " sec" << std::endl;
    std::cout << "Total number of cores:                    " << str->cores.size() << std::endl;
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

    int chrom_index = 0;
    int all_distances[LCP_LEVEL][2*DISTANCE_LENGTH] = {0};  // in order to store overlapping cores as well (as distance will be negative)
    int all_distances_pos[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    int all_lengths[LCP_LEVEL][DISTANCE_LENGTH] = {0};
    std::vector<std::vector<int>> all_larger_distances_vec(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_distances_pos_vec(LCP_LEVEL);
    std::vector<std::vector<int>> all_larger_lengths_vec(LCP_LEVEL);

	// read file
    if ( genome.is_open() ) {  

        std::string sequence, id;
        sequence.reserve(250000000);
        std::chrono::seconds total_duration;
        overflow of;
        
        std::cout << "Program begins" << std::endl;

        while (getline(genome, line)) {

            if (line[0] == '>') {

                chrom_index++;

                // process previous chromosome before moving into new one
                if (sequence.size() != 0) {
                    auto start = std::chrono::high_resolution_clock::now();
                
                    lcp::string *str = new lcp::string(sequence);
                    
                    auto extraction_end = std::chrono::high_resolution_clock::now();
                    auto duration_processing = std::chrono::duration_cast<std::chrono::seconds>(extraction_end - start);

                    summary(0, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, of, str, duration_processing);
                    
                    if (str->cores.size() > 2) {

                        for ( int i = 1; i < LCP_LEVEL; i++ ) {

                            auto start_level = std::chrono::high_resolution_clock::now();

                            str->deepen();
                            
                            auto stop_level = std::chrono::high_resolution_clock::now();
                            auto duration_level = std::chrono::duration_cast<std::chrono::seconds>(stop_level - start_level);
                            duration_processing += std::chrono::seconds(duration_level.count());

                            summary(i, all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, of, str, duration_level);
                        }

                        std::cout << "Processing is done for " << id << std::endl;
                        std::cout << "Length of the processed sequence: " << sequence.size() << std::endl;
                        std::cout << "Total execution time:             " << duration_processing.count() << " sec" << std::endl;
                        std::cout << std::endl;
                    }

                    delete str;

                    total_duration += std::chrono::seconds(duration_processing.count());
                    sequence.clear();
                }

                if ( chrom_index > 23 ) {
                    break;
                }
                
                id = line.substr(1);
                std::cout << "Processing started for " << id << std::endl;
                std::cout << std::endl;
                continue;
                    
            }
            else if (line[0] != '>'){
                sequence += line;
            }
        }

        std::cout << "Core count with len >10k:         " << of.larger_lengths << std::endl;
        std::cout << "Core count with pos_dist >10k:    " << of.larger_pos_distances << std::endl;
        std::cout << "Core count with dist >10k:        " << of.larger_distances << std::endl;
        
        genome.close();
        
        std::cout << "Total execution time of the program:  " << total_duration.count() << " sec" << std::endl;
    }

    std::ofstream outfile ( "detailed_summary.fasta.txt" );
    
    print2file( all_distances, all_distances_pos, all_lengths, all_larger_distances_vec, all_larger_distances_pos_vec, all_larger_lengths_vec, outfile );

    outfile.close();

    return 0;
};
