#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           4
#define KMER_SIZE           15


inline void process(std::string &str ) {
    std::string processed = "";
    for ( std::string::iterator it = str.begin(); it != str.end(); it++ ) {
        if ( (*it) != '-') {
            processed += (*it);
        }
    }
    str = processed;
};


inline void analyze( lcp::string *str1, lcp::string *str2, int &deletion, int &insertion, int &match, int &mismatch ) {

    int m = str1->cores.size(), n = str2->cores.size(), i, j, index;
    int arr1[m], arr2[n];
    
    for ( std::deque<lcp::core*>::iterator it = str1->cores.begin(); it != str1->cores.end(); it++ ) {
        arr1[index] = (*it)->label();
    }
    
    index = 0;
    for ( std::deque<lcp::core*>::iterator it = str2->cores.begin(); it != str2->cores.end(); it++ ) {
        arr2[index] = (*it)->label();
    }

    int dp[m+1][n+1] = {0};
 
    for (i = 0; i <= m; i++) { dp[i][0] = i; }
    for (i = 0; i <= n; i++) { dp[0][i] = i; }    
    

    // calculating the minimum penalty
    for ( i = 1; i <= m; i++ ) {
        for ( j = 1; j <= n; j++ ) {
            if ( arr1[i - 1] == arr2[j - 1] ) { dp[i][j] = dp[i - 1][j - 1]; }
            else { dp[i][j] = std::min( dp[i - 1][j - 1] + 1 , std::min( dp[i - 1][j] + 1, dp[i][j - 1] + 1 ) ); }
        }
    }
 
    // Finding deletion, insertion and mismatch count
    i = m; j = n;

    std::cout << "Finding deletion, insertion and mismatch count" << std::endl;
    while ( !(i == 0 || j == 0)) {
        if (arr1[i - 1] == arr2[j - 1]) { i--; j--; match++; }
        else if (dp[i - 1][j - 1] + 1 == dp[i][j]) { i--; j--; mismatch++; }
        else if (dp[i - 1][j] + 1 == dp[i][j]) { i--; deletion++; }
        else if (dp[i][j - 1] + 1 == dp[i][j]) { j--; insertion++; }
        //else { std::cout << "else" << std::endl; }
    }
};


inline void print2file(int **all_distances, int **all_distances_pos, int **all_lengths, std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, std::ofstream &outfile ) { 
    

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


inline void summary( int *overlapping_counts, int **all_distances, int **all_distances_pos, int **all_lengths, std::vector<std::vector<int>> &all_larger_distances_vec, std::vector<std::vector<int>> &all_larger_distances_pos_vec, std::vector<std::vector<int>> &all_larger_lengths_vec, std::vector<std::chrono::milliseconds> &all_durations, int *all_core_count ) {
    
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

        std::cout << "Level execution time:                     " << ( (double) all_durations[i].count() ) / 1000 << " sec" << std::endl;
        std::cout << "Total number of cores:                    " << all_core_count[i] << std::endl;

        std::cout << "------------------------------------------" << std::endl;

        std::cout << "dist # not in [-10k,10k):                 " << all_larger_distances_vec[i].size() << std::endl;
        std::cout << "dist btw pos # not in [0,10k):            " << all_larger_distances_pos_vec[i].size() << std::endl; 
        std::cout << "length # [0,10k):                         " << all_larger_lengths_vec[i].size() << std::endl;
        std::cout << std::endl;
    }

    std::cout << std::endl; 
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

    // assuming dict_bit_size <= 2 and KMER_SIZE <= 16
    bool uht_set[2 ** (dict_bit_size * KMER_SIZE)] = {false};
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

                int curr_value = 0, index = 0, mask = 2**30 - 1;

                std::string::iterator it = line_original.begin();
                for ( index < KMER_SIZE - 1 && it < line_original.end(); index++, it++ ) {
                    curr_value *= dict_bit_size;
                    curr_value |= coefficients[(*it)];
                }

                for ( ;  it < line_original.end(); it++, index++ ) {
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
