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
std::string format_double(double value, size_t precision = 2) {
    std::stringstream ss;
    ss.imbue(std::locale(""));
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
};


/**
 * @brief Calculates the mean of integers.
 *
 * This function calculates the mean of integers in a given range. It supports
 * processing of standard integers within a predefined range (DISTANCE_LENGTH)
 * and additional larger integers provided as a vector.
 * 
 * @param numbers Array of integers within the predefined range.
 * @param numbersXL Vector of integers outside the predefined range.
 * @return The mean of all provided integers.
 */
double mean(int (&numbers)[DISTANCE_LENGTH], std::vector<int> numbersXL = {}) {
    double sum = 0;
    double count = 0;
    for ( size_t i = 0; i < DISTANCE_LENGTH; i++ ) {
        sum += ( i * numbers[i] );
        count += numbers[i];
    }
    for ( size_t i = 0; i < numbersXL.size(); i++ ) {
        sum += numbersXL[i];
    }
    count += numbersXL.size();
    return sum / count;
};


/**
 * @brief Calculates the standard deviation of integers.
 *
 * This function calculates the standard deviation of integers in a given range.
 * It leverages the mean function for its calculation and handles both standard
 * and larger integers.
 *
 * @param numbers Array of integers within the predefined range.
 * @param numbersXL Vector of integers outside the predefined range.
 * @return The standard deviation of all provided integers.
 */
double stdev(int (&numbers)[DISTANCE_LENGTH], std::vector<int> numbersXL = {}) {
    double mean_value = mean(numbers, numbersXL);
    double count = 0;
    for ( size_t i = 0; i < DISTANCE_LENGTH; i++ ) {
        count += numbers[i];
    }
    count += numbersXL.size();
    double variance = 0;
    for ( size_t i = 0; i < DISTANCE_LENGTH; i++ ) {
        variance += ( ( mean_value - i ) * ( mean_value - i ) * numbers[i] );
    }
    for ( size_t i = 0; i < numbersXL.size(); i++ ) {
        variance += ( ( mean_value - numbersXL[i] ) * ( mean_value - numbersXL[i] ) );
    }
    return sqrt(variance / count);
};


/**
 * @brief Prints a summary of the analysis to the console.
 *
 * Outputs a concise summary of the statistical analysis, including mean and standard
 * deviation of distances and lengths, along with execution times and count of cores
 * for each LCP level.
 *
 * @param sizes An array storing sizes (bytes) of LCP cores found at each level.
 * @param contiguous_counts Array of counts of contiguous cores for each LCP level.
 * @param distances Array of all position distances for each LCP level.
 * @param distancesXL Vector containing larger position distances for each LCP level.
 * @param lengths Array of all lengths for each LCP level.
 * @param lengthsXL Vector containing larger lengths for each LCP level.
 * @param durations Vector containing execution durations for each LCP level.
 * @param core_counts Array of total number of cores for each LCP level.
 * @param distinct_core_count An array string the number of distinct cores found at each level.
 * @param genome_size An size of the gneome that is being processed
 */
void summaryLCP( double (&sizes)[LCP_LEVEL], int (&contiguous_counts)[LCP_LEVEL], int (&distances)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &distancesXL, int (&lengths)[LCP_LEVEL][DISTANCE_LENGTH], std::vector<std::vector<int>> &lengthsXL, std::vector<std::chrono::milliseconds> &durations, int (&core_counts)[LCP_LEVEL], int (&distinct_core_counts)[LCP_LEVEL], size_t genome_size) {

    std::string sep = " & ";
    double previous, current;

    std::cout << "LCP level";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << i + 1;
    }
    std::cout << std::endl;

    // Total Cores
    std::cout << "Total Cores";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_int(core_counts[i]);
    }
    std::cout << std::endl;

    // Contiguous Cores
    std::cout << "Contiguous Cores";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_int(contiguous_counts[i]);
    }
    std::cout << std::endl;

    // Distinct Cores
    std::cout << "Unique Cores (Table)";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_int(distinct_core_counts[i]);
    }
    std::cout << std::endl;

    // Execution Time
    std::cout << "Exec. Time (sec) (Table)";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(((double) durations[i].count()) / 1000);
    }
    std::cout << std::endl;

    // Mean Core Distances
    std::cout << "Mean Core Distances";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(mean(distances[i], distancesXL[i]));
    }
    std::cout << std::endl;

    // Std Dev of Distances
    std::cout << "Std Dev of Distances";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(stdev(distances[i], distancesXL[i]));
    }
    std::cout << std::endl;

    // Mean Core Length
    std::cout << "Mean Core Length";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(mean(lengths[i], lengthsXL[i]));
    }
    std::cout << std::endl;

    // Std Dev of Lengths
    std::cout << "Std Dev of Lengths";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(stdev(lengths[i], lengthsXL[i]));
    }
    std::cout << std::endl;

    // Decrease in Total Counts
    previous = genome_size;
    std::cout << "Decrease in Total Counts";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double( static_cast<double>(core_counts[i]) / previous);
        previous = static_cast<double>(core_counts[i]);
    }
    std::cout << std::endl;

    // Increase in Mean Lengths
    previous = 1;
    std::cout << "Increase in Mean Lengths";
    for (int i = 0; i < LCP_LEVEL; i++) {
        current = mean(lengths[i], lengthsXL[i]);
        std::cout << sep << format_double( current / previous);
        previous = current;
    }
    std::cout << std::endl;

    // Increase in Mean Distances
    previous = 1;
    std::cout << "Increase in Mean Distances";
    for (int i = 0; i < LCP_LEVEL; i++) {
        current = mean(distances[i], distancesXL[i]);
        std::cout << sep << format_double( current / previous);
        previous = current;
    }
    std::cout << std::endl;

    // Total Sizes
    std::cout << "Total Size (GB)*";
    for (int i = 0; i < LCP_LEVEL; i++) {
        std::cout << sep << format_double(sizes[i] / (1024.0 * 1024.0 * 1024.0));
    }
    std::cout << std::endl;
};
