#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           4


inline void process(std::string &str ) {
    std::string processed = "";
    for ( std::string::iterator it = str.begin(); it != str.end(); it++ ) {
        if ( (*it) != '-') {
            processed += (*it);
        }
    }
    str = processed;
}


inline void analyze( std::vector<int> &original_uht_values, std::vector<int> &error_uht_values, int &deletion, int &insertion, int &match, int &mismatch ) {

    // TODO examine distances
    int m = original_uht_values.size(), n = error_uht_values.size(), i, j;

    int dp[m+1][n+1] = {0};
 
    for (i = 0; i <= m; i++) { dp[i][0] = i; }
    for (i = 0; i <= n; i++) { dp[0][i] = i; }    
    

    // calculating the minimum penalty
    for ( i = 1; i <= m; i++ ) {
        for ( j = 1; j <= n; j++ ) {
            if ( original_uht_values[i - 1] == error_uht_values[j - 1] ) { dp[i][j] = dp[i - 1][j - 1]; }
            else { dp[i][j] = std::min( dp[i - 1][j - 1] + 1 , std::min( dp[i - 1][j] + 1, dp[i][j - 1] + 1 ) ); }
        }
    }
 
    // Finding deletion, insertion and mismatch count
    i = m; j = n;

    std::cout << "Finding deletion, insertion and mismatch count" << std::endl;
    while ( !(i == 0 || j == 0)) {
        if (original_uht_values[i - 1] == error_uht_values[j - 1]) { i--; j--; match++; }
        else if (dp[i - 1][j - 1] + 1 == dp[i][j]) { i--; j--; mismatch++; }
        else if (dp[i - 1][j] + 1 == dp[i][j]) { i--; deletion++; }
        else if (dp[i][j - 1] + 1 == dp[i][j]) { j--; insertion++; }
        //else { std::cout << "else" << std::endl; }
    }
};


int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] " << std::endl;
        return -1;  
    }

    std::ifstream input1(argv[1]);
    if (!input1.good()) {
        std::cerr << "Error opening: " << argv[1] << " . You have failed." << std::endl;
        return -1;
    }

    std::ifstream input2(argv[2]);
    if (!input2.good()) {
        std::cerr << "Error opening: " << argv[2] << " . You have failed." << std::endl;
        return -1;
    }

    std::ifstream input3(argv[3]);
    if (!input1.good()) {
        std::cerr << "Error opening: " << argv[3] << " . You have failed." << std::endl;
        return -1;
    }

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string line_original, line_error;

    std::fstream uht_file, genome_original, genome_error;
    uht_file.open(argv[1], std::ios::in);
    genome_original.open(argv[2], std::ios::in);
    genome_error.open(argv[3], std::ios::in);

    bool uht_set[4 ** 15] = {false};
    int uht_value;
    if ( uht_fil ) {
        while ( getline(uht_file, uht_line) ) {
            uht_value = 0;
            for ( std::string::iterator it = uht_line.begin(); it < uht_line.end(); it++ ) {
                uht_value *= dict_bit_size;
                uht_value |= coefficients[(*it)];
            }
            uht_set[uht_value] = true;
        }
    } else {
        exit(-1);
    }

    int read_count = 0;
    int deletion = 0;
    int insertion = 0;
    int match = 0;
    int mismatch = 0;

    std::vector<int> original_uht_values;
    std::vector<int> error_uht_values;
    std::vector<int> original_uht_indeces;
    std::vector<int> error_uht_indeces;

    
	// read file
    if ( genome_original.is_open() && genome_error.is_open() ) {  

        std::cout << "Program begins" << std::endl;

        while ( true ) {
            if ( getline(genome_original, line_original) && getline(genome_error, line_error) ) {

                read_count++;

                // line is id

                // raw sequence
                getline(genome_original, line_original);
                getline(genome_error, line_error);

                process( line_original );

                int curr_value = 0, index = 0, mask = 2**15 - 1;

                std::string::iterator it = line_original.begin();
                for ( index < KMER_SIZE - 1 && it < line_original.end(); index++, it++ ) {
                    curr_value *= dict_bit_size;
                    curr_value |= coefficients[(*it)];
                }

                for ( ;  it < line_original.end(); it++ ) {
                    curr_value *= dict_bit_size;
                    curr_value &= mask;
                    curr_value |= coefficients[(*it)];

                    if ( uht_set[curr_value] ) {
                        original_uht_indeces.push_back(index);
                    }
                }

                curr_value = 0; index = 0;
                it = line_error.begin();
                for ( index < KMER_SIZE - 1 && it < line_error.end(); index++, it++ ) {
                    curr_value *= dict_bit_size;
                    curr_value |= coefficients[(*it)];
                }

                for ( ;  it < line_error.end(); it++ ) {
                    curr_value *= dict_bit_size;
                    curr_value &= mask;
                    curr_value |= coefficients[(*it)];

                    if ( uht_set[curr_value] ) {
                        error_uht_values.push(curr_value);
                    }
                }

                analyze( original_uht_indeces, original_uht_values, error_uht_indeces, error_uht_values, deletion, insertion, match, mismatch );
                
                original_uht_indeces.clear();
                original_uht_values.clear();

                // +
                getline(genome_original, line_original);
                getline(genome_error, line_error);
                
                // quality
                getline(genome_original, line_original);
                getline(genome_error, line_error);
            
            } else {
                break;
            }
        }
                
        genome_original.close();
        genome_error.close();
    }

    std::cout << "Deletion:     " << deletion << std::endl;
    std::cout << "Insertion:    " << insertion << std::endl;
    std::cout << "Match:    " << match << std::endl;
    std::cout << "Mismatch:     " << mismatch << std::endl;

    return 0;
};
