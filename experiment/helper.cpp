/**
 * @file    GenomicDataAnalysis.cpp
 * @brief   Statistical Analysis of Genomic Sequence Alignments
 *
 * This program performs statistical analysis on genomic sequence alignment data.
 * It includes functionality for calculating the mean and standard deviation of distances
 * and lengths in the alignments. The analysis is done at multiple levels, defined by
 * LCP_LEVEL, and caters to a range of distance lengths, specified by DISTANCE_LENGTH.
 * The results are printed to a file and include detailed statistics at each level of
 * analysis.
 *
 */


#include <iostream>
#include <cmath>

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           8

/**
 * @brief Calculates the mean of distances.
 *
 * This function calculates the mean of distances in a given range. It supports
 * processing of standard distances within a predefined range (DISTANCE_LENGTH)
 * and additional larger distances provided as a vector.
 * 
 * @param distances Array of distances within the predefined range.
 * @param larger_distances Vector of distances outside the predefined range.
 * @return The mean of all provided distances.
 */
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


/**
 * @brief Calculates the standard deviation of distances.
 *
 * This function calculates the standard deviation of distances in a given range.
 * It leverages the mean function for its calculation and handles both standard
 * and larger distances.
 *
 * @param distances Array of distances within the predefined range.
 * @param larger_distances Vector of distances outside the predefined range.
 * @return The standard deviation of all provided distances.
 */
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


/**
 * @brief Prints detailed alignment data to a file.
 *
 * Outputs detailed alignment data for each LCP level to a specified file.
 * This includes distances, positions, and lengths of the cores in the alignments.
 * The data is formatted for easy analysis and visualization.
 *
 * @param all_distances Array of all distances for each LCP level.
 * @param all_distances_pos Array of all position distances for each LCP level.
 * @param all_lengths Array of all lengths for each LCP level.
 * @param all_larger_distances_vec Vector containing larger distances for each LCP level.
 * @param all_larger_distances_pos_vec Vector containing larger position distances for each LCP level.
 * @param all_larger_lengths_vec Vector containing larger lengths for each LCP level.
 * @param outfile Reference to the output file stream.
 */
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


/**
 * @brief Prints a summary of the analysis to the console.
 *
 * Outputs a concise summary of the statistical analysis, including mean and standard
 * deviation of distances and lengths, along with execution times and count of cores
 * for each LCP level.
 *
 * @param overlapping_counts Array of counts of overlapping cores for each LCP level.
 * @param all_distances Array of all distances for each LCP level.
 * @param all_distances_pos Array of all position distances for each LCP level.
 * @param all_lengths Array of all lengths for each LCP level.
 * @param all_larger_distances_vec Vector containing larger distances for each LCP level.
 * @param all_larger_distances_pos_vec Vector containing larger position distances for each LCP level.
 * @param all_larger_lengths_vec Vector containing larger lengths for each LCP level.
 * @param all_durations Vector containing execution durations for each LCP level.
 * @param all_core_count Array of total number of cores for each LCP level.
 */
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