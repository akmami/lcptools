#include <fstream>
#include <iostream>
#include <sstream>
#include "../string.cpp"

#define MAX_ROWS            100000
#define MAX_COLUMNS         100000
#define GAP_PENALTY         -2
#define MATCH               1
#define MISMATCH            -1

bool process( std::string &str ) {
    
    size_t writeIndex = 0;

    for (size_t readIndex = 0; readIndex < str.size(); readIndex++) {
        if (str[readIndex] != '-') {
            str[writeIndex++] = str[readIndex];
        }
    }

    str.erase( str.end() - (str.size() - writeIndex ), str.end() );

    return true;
};

bool align(lcp::string* seq1, lcp::string* seq2, int** dp, int& deletion, int& insertion, int& match, int& mismatch ) {
    int rows = seq1->cores.size() + 1;
    int cols = seq2->cores.size() + 1;
   
    for (int i = 1; i < rows; ++i) {
        dp[i][0] = dp[i - 1][0] + 1;
    }

    for (int j = 1; j < cols; ++j) {
        dp[0][j] = dp[0][j - 1] + 1;
    }

    for (int i = 1; i < rows; ++i) {
        for (int j = 1; j < cols; ++j) {

            dp[i][j] = std::max(
                std::max(
                    dp[i - 1][j] + GAP_PENALTY ,
                    dp[i][j - 1] + GAP_PENALTY
                ), 
                    dp[i - 1][j - 1] + ( seq1->cores.at(i - 1) == seq2->cores.at(j - 1) ? MATCH : MISMATCH )
                );
        }
    }

    // Traceback to find the alignment
    int i = rows - 1;
    int j = cols - 1;

    while (i > 0 || j > 0) {

        if (i > 0 && j > 0 && dp[i][j] == dp[i - 1][j - 1] + ( seq1->cores.at(i - 1) == seq2->cores.at(j - 1) ? MATCH : MISMATCH ) ) {
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

    return true;
};

int main(int argc, char **argv) {

    if (argc < 3) {
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

    int deletion = 0, insertion = 0, match = 0, mismatch = 0, read_count = 0;

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string line, seq1, seq2;

    std::fstream maf_file;
    maf_file.open(argv[1], std::ios::in);

    std::ofstream output("output.txt");

    if (!output.is_open()) {
        std::cerr << "Error opening file: " << argv[2] << std::endl;
        return -1;
    }

	// read file
    if ( maf_file.is_open() ) {  

        std::cout << "Program begins" << std::endl;

        while ( std::getline(maf_file, line) ) {
            
            read_count++;

            // line is 'a'

            // Raw Sequence 1
            std::getline(maf_file, line);

            std::istringstream iss(line);

            // Ignore the first six columns
            for (int j = 0; j < 6; ++j) {
                iss >> seq1; 
            }
            // Access the 7th column
            iss >> seq1;

            // Raw Sequence 2
            std::getline(maf_file, line);
            
            iss.clear();
            iss.str(line);

            // Ignore the first six columns
            for (int j = 0; j < 6; ++j) {
                iss >> seq2; 
            }

            // Access the 7th column
            iss >> seq2;

            // empty line
            getline(maf_file, line);

            process(seq1);
            process(seq2);

            lcp::string *str1 = new lcp::string(seq1);
            lcp::string *str2 = new lcp::string(seq2);

            for (int i = 1; i < lcp_level; i++) {
                str1->deepen();
                str2->deepen();
            }

            align(str1, str2, dp, deletion, insertion, match, mismatch);

            std::cout << str1 << std::endl;
            std::cout << str2 << std::endl;
        }
                
        maf_file.close();
    }

    std::cout << "Deletion:     " << deletion << std::endl;
    std::cout << "Insertion:    " << insertion << std::endl;
    std::cout << "Match:    " << match << std::endl;
    std::cout << "Mismatch:     " << mismatch << std::endl;

    for (int i = 0; i < MAX_ROWS; ++i) {
        delete dp[i];
    }
    delete dp;

    return 0;
};
