#ifndef   ENCODING_H
#define   ENCODING_H

#include <map>
#include <fstream>
#include <iostream>

namespace lcp {
    
    extern int coefficients[128];
    extern int reverse_complement_coefficients[128];
    extern char characters[128];
    extern int dict_bit_size;

    /**
     * @brief Displays the alphabet encoding summary including coefficients and dictionary bit size.
     */
    void encoding_summary();

    /**
     * @brief Initializes the encoding coefficients for standard DNA bases (A, C, G, T) and their 
     *        reverse complements. Sets default values for coefficients and dictionary bit size.
     * @param verbose If true, prints the encoding summary after initialization.
     * @return Always returns 0 upon successful initialization.
     */
    int init_coefficients(bool verbose=false);

    /**
     * @brief Initializes the encoding coefficients and reverse complement coefficients using 
     *        user-specified mappings. Calculates the dictionary bit size based on the maximum encoding 
     *        value.
     * @param map A mapping of characters to encoding values.
     * @param rc_map A mapping of characters to reverse complement encoding values.
     * @param verbose If true, prints the encoding summary after initialization.
     * @return Always returns 0 upon successful initialization.
     * @throws std::invalid_argument if any value in the map or reverse complement map is negative.
     */
    int init_coefficients(std::map<char, int> map, std::map<char, int> rc_map, bool verbose=false);

    /**
     * @brief Initializes the encoding coefficients by reading them from a file.
     *        The file must contain character, encoding, and reverse complement encoding on each line.
     * @param encoding_file Path to the file containing the character encodings.
     * @param verbose If true, prints the encoding summary after initialization.
     * @return Always returns 0 upon successful initialization.
     * @throws std::invalid_argument if any invalid data is found in the file.
     */
    int init_coefficients(std::string encoding_file, bool verbose=false);
};

#endif
