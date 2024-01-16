#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define MAX_ROWS            100000
#define MAX_COLUMNS         100000
#define GAP_PENALTY         -4
#define MATCH               5
#define MISMATCH            -4

//
// The main assumption here is that given strings/sequences cannot exceed 100000.
// You can increase MAX_ROWS along with MAX_COLUMN 
//
// REMARK: Matrix is constantly being reused for pairwise alignment
//

void analyze(lcp::string* seq1, lcp::string* seq2, int** dp, int& deletion, int& insertion, int& match, int& mismatch ) {
    int rows = seq1->cores.size() + 1;
    int cols = seq2->cores.size() + 1;
   
    dp[0][0] = 0;

    for (int i = 1; i < rows; ++i) {
        dp[i][0] = dp[i - 1][0] + GAP_PENALTY;
    }

    for (int j = 1; j < cols; ++j) {
        dp[0][j] = dp[0][j - 1] + GAP_PENALTY;
    }

    for (int i = 1; i < rows; ++i) {
        for (int j = 1; j < cols; ++j) {

            dp[i][j] = std::max(
                std::max(
                    dp[i - 1][j] + GAP_PENALTY ,
                    dp[i][j - 1] + GAP_PENALTY
                ), 
                    dp[i - 1][j - 1] + ( seq1->cores.at(i - 1)->label() == seq2->cores.at(j - 1)->label() ? MATCH : MISMATCH )
                );
        }
    }

    // Traceback to find the alignment
    int i = rows - 1;
    int j = cols - 1;

    while (i > 0 || j > 0) {

        if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + ( seq1->cores.at(i - 1)->label() == seq2->cores.at(j - 1)->label() ? MATCH : MISMATCH ) ) {
            // Match or mismatch
            if ( seq1->cores.at(i - 1) == seq2->cores.at(j - 1) ) {
                ++match;
            } else {
                ++mismatch;
            }
            --i;
            --j;
        } else if (i > 0 && dp[i][j] == dp[i - 1][j] + GAP_PENALTY) {
            ++deletion;
            --i;
        } else {
            ++insertion;
            --j;
        }
    }
};


int main(int argc, char **argv) {

    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [lcp-level]" << std::endl;
        return -1;  
    }

    std::ifstream input1(argv[1]);
    if (!input1.good()) {
        std::cerr << "Error opening: " << argv[1] << " . You have failed." << std::endl;
        return -1;
    }

    int lcp_level = atoi(argv[2]);

    int** dp = new int*[MAX_ROWS];
    for (int i = 0; i < MAX_ROWS; ++i) {
        dp[i] = new int[MAX_COLUMNS];
    }
    
    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string str1, str2;

    std::fstream maf_file;
    maf_file.open(argv[1], std::ios::in);

    int read_count;
    int deletion;
    int insertion;
    int match;
    int mismatch;
    
	// read file
    if ( maf_file.is_open() ) {  

        std::cout << "Deletion  Insertion   Match   Mismatch" << std::endl;

        while ( getline(maf_file, str1) ) {
            
            read_count = 0;
            deletion = 0;
            insertion = 0;
            match = 0;
            mismatch = 0;

            // other sequence
            getline(maf_file, str2);

            lcp::string *str_original = new lcp::string(str1);
            lcp::string *str_error = new lcp::string(str2);

            for ( int i = 1; i < lcp_level; i++ ) {
                str_original->deepen();
                str_error->deepen();
            }

            analyze( str_original, str_error, dp, deletion, insertion, match, mismatch );

            std::cout << str_original << std::endl;
            std::cout << str_error << std::endl;

            delete str_original;
            delete str_error;

            std::cout << deletion << '\t' << insertion << '\t' << match << '\t' << mismatch << std::endl;

            if ( read_count++ == 4 ) {
                break;
            }
        }
                
        maf_file.close();
    }

    for (int i = 0; i < MAX_ROWS; ++i) {
        delete dp[i];
    }
    delete dp;

    return 0;
};
