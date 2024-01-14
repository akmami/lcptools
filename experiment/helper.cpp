#include <iostream>
#include <cmath>

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           8


double mean(int (&distances)[DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
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


double stdev(int (&distances)[DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
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


double mean_shifted(int (&distances)[2*DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
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


double stdev_shifted(int (&distances)[2*DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
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


void print2file(int (&all_distances)[LCP_LEVEL][2*DISTANCE_LENGTH], int (&all_distances_pos)[LCP_LEVEL][DISTANCE_LENGTH], int (&all_lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, std::ofstream &outfile ) { 
    
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


void summary( int (&overlapping_counts)[LCP_LEVEL], int (&all_distances)[LCP_LEVEL][2*DISTANCE_LENGTH], int (&all_distances_pos)[LCP_LEVEL][DISTANCE_LENGTH], int (&all_lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, std::vector<std::chrono::milliseconds> &all_durations, int (&all_core_count)[LCP_LEVEL]) {

    for ( int i = 0; i < LCP_LEVEL; i++ ) {

        std::cout << "Level: " << i << std::endl;

        std::cout << "Level execution time:                     " << ( (double) all_durations[i].count() ) / 1000 << " sec" << std::endl;
        std::cout << "Total number of cores:                    " << all_core_count[i] << std::endl;
        std::cout << "Overlapping core counts:                  " << overlapping_counts[i] << std::endl;

        std::cout << "------------------------------------------" << std::endl;

        std::cout << "Mean of distances btw cores (w'out):      " << mean_shifted(all_distances[i]) << std::endl; 
        std::cout << "Std of distances btw cores (w'out):       " << stdev_shifted(all_distances[i]) << std::endl;
        std::cout << "Mean of distances btw starts (w'out):     " << mean(all_distances_pos[i]) << std::endl;
        std::cout << "Std of distances btw starts (w'out):      " << stdev(all_distances_pos[i]) << std::endl;
        std::cout << "Mean of lengths (w'out):                  " << mean(all_lengths[i]) << std::endl;
        std::cout << "Std of lengths (w'out):                   " << stdev(all_lengths[i]) << std::endl;

        std::cout << "------------------------------------------" << std::endl;
        
        std::cout << "Mean of distances btw cores (with):       " << mean_shifted(all_distances[i], all_larger_distances_vec[i]) << std::endl;
        std::cout << "Std of distances btw cores (with):        " << stdev_shifted(all_distances[i], all_larger_distances_vec[i]) << std::endl;
        std::cout << "Mean of distances btw starts (with):      " << mean(all_distances_pos[i], all_larger_distances_pos_vec[i]) << std::endl;
        std::cout << "Std of distances btw starts (with):       " << stdev(all_distances_pos[i], all_larger_distances_pos_vec[i]) << std::endl;
        std::cout << "Mean of lengths (with):                   " << mean(all_lengths[i], all_larger_lengths_vec[i]) << std::endl;
        std::cout << "Std of lengths (with):                    " << stdev(all_lengths[i], all_larger_lengths_vec[i]) << std::endl;

        std::cout << "------------------------------------------" << std::endl;
            
        std::cout << "dist # not in [-10k,10k):                 " << all_larger_distances_vec[i].size() << std::endl;
        std::cout << "dist btw pos # not in [0,10k):            " << all_larger_distances_pos_vec[i].size() << std::endl; 
        std::cout << "length # [0,10k):                         " << all_larger_lengths_vec[i].size() << std::endl;
        std::cout << std::endl;
    }
};