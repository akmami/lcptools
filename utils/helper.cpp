/**
 * @file    helper.cpp
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
#include <fstream>
#include <cmath>
#include <vector>
#include <chrono>

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           8
#define KMER_SIZE           10
#define WINDOW_SIZE         15


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
    for ( uint i = 0; i < larger_distances.size(); i++ ) {
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
    for ( uint i = 0; i < larger_distances.size(); i++ ) {
        variance += ( m - larger_distances[i] ) * ( m - larger_distances[i] );
    }
    return sqrt(variance / count);
};


/**
 * @brief Calculates the shifted mean of a set of distances.
 *
 * This function computes the mean of an array of distances, where each distance is
 * shifted by subtracting a predefined offset (DISTANCE_LENGTH). It is designed to
 * handle cases where distances are symmetrically distributed around DISTANCE_LENGTH.
 * The function also accommodates an optional vector of additional distances that are
 * not subject to the shifting but are included in the mean calculation. The vector
 * contains elements that are not in interval of [-DISTANCE_LENGTH, DISTANCE_LENGTH].
 *
 * @param distances A reference to an array of integers, size 2*DISTANCE_LENGTH,
 *        containing the primary set of distances. Each distance is adjusted by
 *        subtracting DISTANCE_LENGTH before being included in the mean calculation.
 * @param larger_distances (Optional) A vector of integers for additional distances.
 *        These distances are included directly in the mean calculation without any
 *        shifting.
 *
 * @return The mean of the combined set of distances as a double. This mean includes
 *         both the shifted values from the array and the direct values from the vector.
 */
double mean_shifted(int (&distances)[2*DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = 0;
    double count = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        m += (i-DISTANCE_LENGTH) * distances[i];
        count += distances[i];
    }
    for ( uint i = 0; i < larger_distances.size(); i++ ) {
        m += larger_distances[i];
    }
    count += larger_distances.size();
    return m / count;
};


/**
 * @brief Calculates the shifted standard deviation of a set of distances.
 *
 * This function computes the standard deviation for an array of distances, 
 * with each distance shifted by subtracting a predefined offset (DISTANCE_LENGTH).
 * It is particularly useful in scenarios where the distances are symmetrically 
 * distributed around DISTANCE_LENGTH. The function also supports an optional vector 
 * of additional distances, which are included directly in the standard deviation 
 * calculation without shifting.
 *
 * The standard deviation is calculated based on the shifted mean of the distances,
 * which is computed using the mean_shifted function. This ensures consistency in 
 * the statistical analysis of the data.
 *
 * @param distances A reference to an array of integers, size 2*DISTANCE_LENGTH,
 *        containing the primary set of distances. Each distance in this array 
 *        is adjusted by subtracting DISTANCE_LENGTH before contributing to the
 *        standard deviation calculation.
 * @param larger_distances (Optional) A vector of integers for additional distances.
 *        These distances are included directly in the standard deviation calculation 
 *        without any shifting.
 *
 * @return The standard deviation of the combined set of distances as a double. This 
 *         includes both the shifted values from the array and the direct values from 
 *         the vector, based on the shifted mean.
 */
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
    for ( uint i = 0; i < larger_distances.size(); i++ ) {
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
        for ( uint j = 0; j < all_larger_distances_vec[i].size(); j++ ) {
            outfile << all_larger_distances_vec[i][j] << ',';
        }
        outfile << std::endl;

        outfile << "Distances btw pos ( core_curr->pos - core_prev->pos ) " << std::endl;
        for ( int j = 0; j < DISTANCE_LENGTH; j++ ) {
            for ( int k = 0; k < all_distances_pos[i][j]; k++ ) {
                outfile << j << ',';
            }
        }
        for ( uint j = 0; j < all_larger_distances_pos_vec[i].size(); j++ ) {
            outfile << all_larger_distances_pos_vec[i][j] << ',';
        }
        outfile << std::endl;

        outfile << "Lengths ( core_curr->end - core_curr->pos ) " << std::endl;
        for ( int j = 0; j < DISTANCE_LENGTH; j++ ) {
            for ( int k = 0; k < all_lengths[i][j]; k++ ) {
                outfile << j << ',';
            }
        }
        for ( uint j = 0; j < all_larger_lengths_vec[i].size(); j++ ) {
            outfile << all_larger_lengths_vec[i][j] << ',';
        }

        outfile << std::endl << std::endl;
    }
};


/**
 * @brief Prints a summary of statistics for distances between minimizers.
 *
 * This function outputs a summary of the statistical analysis of distances between
 * minimizers. It includes the execution time, total count of minimizers, and the
 * statistical measures (mean and standard deviation) of distances. The statistics
 * are computed both with and without considering additional larger distances that
 * fall outside a predefined range.
 *
 * The summary is printed to the standard output (console) and is formatted to
 * provide a clear and concise overview of the data.
 *
 * @param distances A reference to an array of integers, size DISTANCE_LENGTH,
 *        representing the primary set of distances between minimizers.
 * @param all_larger_distances_vec A vector of integers representing distances
 *        between minimizers that are outside the predefined range.
 * @param total_duration The total execution time of the minimizer analysis,
 *        represented as a duration in milliseconds.
 * @param total_count The total number of minimizers analyzed.
 */
void summaryMimimizer( int (&distances)[DISTANCE_LENGTH], std::vector<int> &all_larger_distances_vec, std::chrono::milliseconds total_duration, size_t total_count ) {
    
    std::cout << "Level execution time:                         " << ( (double) total_duration.count() ) / 1000 << " sec" << std::endl;
    std::cout << "Total number of minimizers:                   " << total_count << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    std::cout << "Mean of distances btw minimizers (w'out):     " << mean(distances) << std::endl; 
    std::cout << "Std of distances btw minimizers (w'out):      " << stdev(distances) << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
        
    std::cout << "Mean of distances btw minimizers (with):      " << mean(distances, all_larger_distances_vec) << std::endl;
    std::cout << "Std of distances btw minimizers (with):       " << stdev(distances, all_larger_distances_vec) << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    std::cout << "dist # not in [-10k,10k):                     " << all_larger_distances_vec.size() << std::endl;
        
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
void summaryLCP( int (&overlapping_counts)[LCP_LEVEL], int (&all_distances)[LCP_LEVEL][2*DISTANCE_LENGTH], int (&all_distances_pos)[LCP_LEVEL][DISTANCE_LENGTH], int (&all_lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, std::vector<std::chrono::milliseconds> &all_durations, int (&all_core_count)[LCP_LEVEL]) {

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
        std::cout << "length # not in [0,10k):                  " << all_larger_lengths_vec[i].size() << std::endl;
        std::cout << std::endl;
    }
};