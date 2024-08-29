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
#include <iomanip>
#include <sstream>


#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           8
#define KMER_SIZE           10
#define WINDOW_SIZE         15


/**
 * @brief Formats an integer with thousands separators for better readability.
 *
 * This function converts an integer to a string representation that includes thousands
 * separators, based on the current locale settings. The resulting string makes large
 * numbers more readable by grouping digits into thousands using separators (e.g., commas
 * in English locales).
 *
 * @param value The integer to be formatted.
 * @return A std::string containing the formatted integer with thousands separators.
 *
 * Example:
 *     format_int(1234567) returns "1,234,567" (in an English locale).
 */
std::string format_int(int value) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << value;
    return ss.str();
};


/**
 * @brief Formats a double value with two decimal places and thousands separators.
 *
 * This function converts a double to a string representation that includes thousands
 * separators and ensures that the number is formatted to two decimal places. The
 * thousands separators are applied based on the current locale settings, making the
 * number easier to read, especially for large values.
 *
 * @param value The double value to be formatted.
 * @return A std::string containing the formatted double with thousands separators
 *         and two decimal places.
 *
 * Example:
 *     format_double(1234567.89123) returns "1,234,567.89" (in an English locale).
 */
std::string format_double(double value) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << std::setprecision(2) << value;
    return ss.str();
};


/**
 * @brief Calculates the mean of distances.
 *
 * This function calculates the mean of distances in a given range. It supports
 * processing of standard distances within a predefined range (DISTANCE_LENGTH)
 * and additional larger distances provided as a vector.
 * 
 * @param distances Array of distances within the predefined range.
 * @param distancesXL Vector of distances outside the predefined range.
 * @return The mean of all provided distances.
 */
double mean(int (&distances)[DISTANCE_LENGTH], std::vector<int> distancesXL = {}) {
    double sum = 0;
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        sum += i * distances[i];
        count += distances[i];
    }
    for ( uint i = 0; i < distancesXL.size(); i++ ) {
        sum += distancesXL[i];
    }
    count += distancesXL.size();
    return sum / count;
};


/**
 * @brief Calculates the standard deviation of distances.
 *
 * This function calculates the standard deviation of distances in a given range.
 * It leverages the mean function for its calculation and handles both standard
 * and larger distances.
 *
 * @param distances Array of distances within the predefined range.
 * @param distancesXL Vector of distances outside the predefined range.
 * @return The standard deviation of all provided distances.
 */
double stdev(int (&distances)[DISTANCE_LENGTH], std::vector<int> distancesXL = {}) {
    double mean = mean(distances, distancesXL);
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        count += distances[i];
    }
    count += distancesXL.size();
    double variance = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        variance += ( mean - i ) * ( mean - i ) * distances[i];
    }
    for ( uint i = 0; i < distancesXL.size(); i++ ) {
        variance += ( mean - distancesXL[i] ) * ( mean - distancesXL[i] );
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
 * @param distances Array of all distances for each LCP level.
 * @param distancesXL Vector containing larger distances for each LCP level.
 * @param lengths Array of all lengths for each LCP level.
 * @param lengthsXL Vector containing larger lengths for each LCP level.
 * @param outfile Reference to the output file stream.
 */
void print2file(int (&distances)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &distancesXL, int (&lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &lengthsXL, std::ofstream &outfile ) { 
    
    for ( int i = 0; i < LCP_LEVEL; i++ ) {
        
        outfile << "Level " << i + 1 << std::endl;

        outfile << "Distances ( core_curr->pos - core_prev->pos ) " << std::endl;
        for ( int j = 0; j < DISTANCE_LENGTH; j++ ) {
            for ( int k = 0; k < distances[i][j]; k++ ) {
                outfile << j - DISTANCE_LENGTH << ',';
            }
        }
        for ( uint j = 0; j < distancesXL[i].size(); j++ ) {
            outfile << distancesXL[i][j] << ',';
        }
        outfile << std::endl;

        outfile << "Lengths ( core_curr->end - core_curr->pos ) " << std::endl;
        for ( int j = 0; j < DISTANCE_LENGTH; j++ ) {
            for ( int k = 0; k < lengths[i][j]; k++ ) {
                outfile << j << ',';
            }
        }
        for ( uint j = 0; j < lengthsXL[i].size(); j++ ) {
            outfile << lengthsXL[i][j] << ',';
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
 * @param distancesXL A vector of integers representing distances
 *        between minimizers that are outside the predefined range.
 * @param total_duration The total execution time of the minimizer analysis,
 *        represented as a duration in milliseconds.
 * @param total_count The total number of minimizers analyzed.
 */
void summaryMimimizer( int (&distances)[DISTANCE_LENGTH], std::vector<int> &distancesXL, std::chrono::milliseconds total_duration, size_t total_count ) {
    
    std::cout << "Level execution time:                         " << ( (double) total_duration.count() ) / 1000 << " sec" << std::endl;
    std::cout << "Total number of minimizers:                   " << total_count << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    std::cout << "Mean of distances btw minimizers (w'out):     " << mean(distances) << std::endl; 
    std::cout << "Std of distances btw minimizers (w'out):      " << stdev(distances) << std::endl;

    std::cout << "----------------------------------------------" << std::endl;
        
    std::cout << "Mean of distances btw minimizers (with):      " << mean(distances, distancesXL) << std::endl;
    std::cout << "Std of distances btw minimizers (with):       " << stdev(distances, distancesXL) << std::endl;

    std::cout << "----------------------------------------------" << std::endl;

    std::cout << "dist # not in [-10k,10k):                     " << distancesXL.size() << std::endl;
        
};

/**
 * @brief Prints a summary of the analysis to the console.
 *
 * Outputs a concise summary of the statistical analysis, including mean and standard
 * deviation of distances and lengths, along with execution times and count of cores
 * for each LCP level.
 *
 * @param overlapping_counts Array of counts of overlapping cores for each LCP level.
 * @param distances Array of all position distances for each LCP level.
 * @param distancesXL Vector containing larger position distances for each LCP level.
 * @param lengths Array of all lengths for each LCP level.
 * @param lengthsXL Vector containing larger lengths for each LCP level.
 * @param durations Vector containing execution durations for each LCP level.
 * @param core_counts Array of total number of cores for each LCP level.
 */
void summaryLCP( int (&overlapping_counts)[LCP_LEVEL], int (&distances)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &distancesXL, int (&lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &lengthsXL, std::vector<std::chrono::milliseconds> &durations, int (&core_counts)[LCP_LEVEL]) {

    std::cout << "Metric";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << i + 1;
    }
    std::cout << std::endl;

    // Total Cores
    std::cout << "Total Cores";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_int(core_counts[i]);
    }
    std::cout << std::endl;

    // Overlapping Cores
    std::cout << "Overlapping Cores";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_int(overlapping_counts[i]);
    }
    std::cout << std::endl;

    // Execution Time
    std::cout << "Execution Time (sec)";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_double(((double) durations[i].count()) / 1000);
    }
    std::cout << std::endl;

    // Mean Distances Starts (with)
    std::cout << "Mean of Pos Difference";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_double(mean(distances[i], distancesXL[i]));
    }
    std::cout << std::endl;

    // Std Distances Starts (with)
    std::cout << "Std of Pos Difference";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_double(stdev(distances[i], distancesXL[i]));
    }
    std::cout << std::endl;

    // Mean Lengths (with)
    std::cout << "Mean of Lengths";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_double(mean(lengths[i], lengthsXL[i]));
    }
    std::cout << std::endl;

    // Std Lengths (with)
    std::cout << "Std of Lengths";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_double(stdev(lengths[i], lengthsXL[i]));
    }
    std::cout << std::endl;

    std::cout << "Pos Diff >10K Count";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_int(distancesXL[i].size());
    }
    std::cout << std::endl;

    std::cout << "Length >10K Count";
    for (int i = 0; i < LCP_LEVEL; ++i) {
        std::cout << "," << format_int(lengthsXL[i].size());
    }
    std::cout << std::endl;
};