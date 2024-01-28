#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           4
#define KMER_SIZE           15
#define WINDOW_SIZE         10


inline void process(std::string &str ) {
    std::string processed = "";
    for ( std::string::iterator it = str.begin(); it != str.end(); it++ ) {
        if ( (*it) != '-') {
            processed += (*it);
        }
    }
    str = processed;
}


inline void analyze( std::vector<int> &minimizers1, std::vector<int> &minimizers2, int &deletion, int &insertion, int &match, int &mismatch ) {

    int m = minimizers1.size(), n = minimizers2.size(), i, j, index;
    int arr1[m], arr2[n];
    
    for ( std::vector<int>::iterator it = minimizers1.begin(); it != minimizers1.end(); it++ ) {
        arr1[index] = (*it)->label();
    }
    
    index = 0;
    for ( std::vector<int>::iterator it = minimizers2.begin(); it != minimizers2.end(); it++ ) {
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

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string line_original, line_error;

    std::fstream genome_original, genome_error;
    genome_original.open(argv[1], std::ios::in);
    genome_error.open(argv[2], std::ios::in);

    int read_count = 0;
    int deletion = 0;
    int insertion = 0;
    int match = 0;
    int mismatch = 0;
    
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
                
                std::vector<int> str_original;
                std::vector<int> str_error;
                
                int kmers1[genome_original.size() - kmer_size + 1];
                int curr_value = 0, mask = 2 ** 30 - 1;
                std::string::iterator it = genome_original.begin();
                for ( ; it + kmer_size < genome_original.end(); it++ ) {
                    curr_value *= dict_bit_size;
                    curr_value |= coefficients[(*it)];
                }

                int kmers2[genome_error.size() - kmer_size + 1];
                curr_value = 0;
                it = genome_error.begin();
                for ( ; it + kmer_size < genome_error.end(); it++ ) {
                    curr_value *= dict_bit_size;
                    curr_value |= coefficients[(*it)];
                }
                
                bool isMinimizer;
                for ( int i = WINDOW_SIZE; i + kmer_size + WINDOW_SIZE < genome_original.size(); i++) {
                    isMinimizer + true;
                    for ( int j = i - 1; i - WINDOW_SIZE < j; j-- ) {
                        if ( kmers1[j] < kmers1[i] ) {
                            isMinimizer = false;
                            break;
                        }
                    }
                    for ( int j = i + 1; j < i + WINDOW_SIZE ; j++ ) {
                        if ( kmers1[j] < kmers1[i] ) {
                            isMinimizer = false;
                            break;
                        }
                    }

                    if ( isMinimizer )
                        str_original.push_back(kmers1[i]);
                }

                for ( int i = WINDOW_SIZE; i + kmer_size + WINDOW_SIZE < genome_error.size(); i++) {
                    isMinimizer + true;
                    for ( int j = i - 1; i - WINDOW_SIZE < j; j-- ) {
                        if ( kmers2[j] < kmers2[i] ) {
                            isMinimizer = false;
                            break;
                        }
                    }
                    for ( int j = i + 1; j < i + WINDOW_SIZE ; j++ ) {
                        if ( kmers2[j] < kmers2[i] ) {
                            isMinimizer = false;
                            break;
                        }
                    }

                    if ( isMinimizer )
                        str_error.push_back(kmers2[i]);
                }
                
                analyze( str_original, str_error, deletion, insertion, match, mismatch );

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
