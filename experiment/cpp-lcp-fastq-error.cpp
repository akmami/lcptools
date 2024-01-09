#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           4


inline void analyze( lcp::string* str1, lcp::string* str2, int &deletion, int &insertion, int &match, int &mismatch ) {

    int m = str1->cores.size(), n = str2->cores.size(), i, j, index;
    int arr1[m], arr2[n];
    
    index = 0;
    for ( std::deque<lcp::core*>::iterator it = str1->cores.begin(); it != str1->cores.end(); it++, index++ ) {
        arr1[index] = (*it)->label();
    }
    
    index = 0;
    for ( std::deque<lcp::core*>::iterator it = str2->cores.begin(); it != str2->cores.end(); it++, index++ ) {
        arr2[index] = (*it)->label();
    }
    
    int **dp = new int*[m+1];
    for ( int i = 0; i < m+1; i++ ) {
        dp[i] = new int[n+1];
    }
 
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

    while ( !(i == 0 || j == 0)) {
        if (arr1[i - 1] == arr2[j - 1]) { i--; j--; match++; }
        else if (dp[i - 1][j - 1] + 1 == dp[i][j]) { i--; j--; mismatch++; }
        else if (dp[i - 1][j] + 1 == dp[i][j]) { i--; deletion++; }
        else if (dp[i][j - 1] + 1 == dp[i][j]) { j--; insertion++; }
        //else { std::cout << "else" << std::endl; }
    }
    
    for ( int i = 0; i < m+1; i++ ) {
        delete dp[i];
    }
    delete [] dp;
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
    
    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string line, str1, str2;

    std::fstream maf_file;
    maf_file.open(argv[1], std::ios::in);

    int read_count = 0;
    int deletion = 0;
    int insertion = 0;
    int match = 0;
    int mismatch = 0;
    
	// read file
    if ( maf_file.is_open() ) {  

        std::cout << "Program begins" << std::endl;

        while ( getline(maf_file, line) ) {
            
            read_count++;

            // line is 'a'

            // raw sequence
            getline(maf_file, str1);
            getline(maf_file, str2);

            process( str1 );
            process( str2 );

            lcp::string *str_original = new lcp::string(str1);
            lcp::string *str_error = new lcp::string(str2);

            for ( int i = 1; i < LCP_LEVEL; i++ ) {
                str_original->deepen();
                str_error->deepen();
            }

            analyze( str_original, str_error, deletion, insertion, match, mismatch );

            delete str_original;
            delete str_error;

            // empty line
            getline(maf_file, line);
        }
                
        maf_file.close();
    }

    std::cout << "Deletion:     " << deletion << std::endl;
    std::cout << "Insertion:    " << insertion << std::endl;
    std::cout << "Match:    " << match << std::endl;
    std::cout << "Mismatch:     " << mismatch << std::endl;

    return 0;
};
