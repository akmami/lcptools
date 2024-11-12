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
#include <string>

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           8


// to represent 15 chars = uint32_t, 31 chars = uint64_t, 63 chars = unsigned __int128
typedef uint32_t kmer_type;


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
 * @brief Calculates the mean of distances.
 *
 * This function computes the weighted mean of an array of distances. Each index
 * in the array corresponds to a distance, and the value at that index represents
 * how many times that distance occurs.
 *
 * @param distances Pointer to an array of integers where each element represents
 *                  the count of occurrences of the corresponding index distance.
 * @param n         The size of the distances array.
 * @return The weighted mean of the distances.
 */
double mean(int* distances, int n) {
    double numOfDistances = 0;
    double totalDistance = 0;
    for (int i = 0; i < n; i++)
    {
        totalDistance += i * distances[i];
        numOfDistances += distances[i];
    }
    return totalDistance / numOfDistances;
};


/**
 * @brief Calculates the standard deviation of distances.
 *
 * This function computes the standard deviation of distances based on their
 * frequency in the array. The mean value is provided as a parameter for use in the
 * calculation.
 *
 * @param distances Pointer to an array of integers representing the counts of each
 *                  distance.
 * @param n         The size of the distances array.
 * @param mean      The precomputed mean of the distances.
 * @return The standard deviation of the distances.
 */
double stdev(int* distances, int n, double mean) {
    double sum = 0;
    double N = 0;
    for (int i = 0; i < n; i++)
    {
        N += distances[i];
        sum += pow(((i) - mean), 2) * distances[i];
    }
    return sqrt(sum / N);
};


/**
 * @brief Encodes a substring of DNA characters into a unique unsigned integer.
 *
 * This function encodes a sequence of characters (typically representing nucleotides
 * in a DNA string) using a predefined mapping into a 2-bit encoded integer for each
 * character. The final encoded value is returned as an unsigned integer.
 *
 * @param map    Pointer to an array where each index represents the mapping of a character.
 * @param begin  Iterator to the beginning of the substring to encode.
 * @param end    Iterator to the end of the substring to encode.
 * @return A unique unsigned integer representing the encoded DNA sequence.
 */
kmer_type encode(int* map, std::string::iterator begin, std::string::iterator end) {
    kmer_type res = 0;
    for ( std::string::iterator it = begin; it < end; it++ )
    {
        res *= 4;
        res |= map[static_cast<size_t>(*it)];
    }
    return res;
};


/**
 * @brief Initializes a character map for nucleotide encoding.
 *
 * This function initializes a mapping of DNA characters (A, T, G, C) and their lowercase
 * counterparts (a, t, g, c) into integers (0, 3, 2, 1, respectively). All other characters
 * are mapped to 0 by default.
 *
 * @param map A 128-element integer array where each index corresponds to a character
 *            in the ASCII table.
 */
void init_map(int map[128]) {
    for ( int i = 0; i < 128; i++ )
    {
        map[i] = 0;
    }

    map['A'] = 0;
    map['T'] = 3;
    map['G'] = 2;
    map['C'] = 1;
    // Assume there are soft masks
    map['a'] = 0;
    map['t'] = 3;
    map['g'] = 2;
    map['c'] = 1;
};
