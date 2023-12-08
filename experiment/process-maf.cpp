#include <fstream>


inline void process(std::string &str ) {
    
    int index = str.find_last_of(' ');
    std::string processed = "";
    
    for ( std::string::iterator it = str.begin() + index + 1; it != str.end(); it++ ) {
        if ( (*it) != '-') {
            processed += (*it);
        }
        if ( (*it) == 'N' ) {
            str = "";
            return;
        }
    }
    str = processed;
}


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
    std::string line, str;

    std::fstream maf_file;
    maf_file.open(argv[1], std::ios::in);

    std::ofstream output(argv[2]);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << argv[2] << std::endl;
        return -1;
    }
    
	// read file
    if ( maf_file.is_open() ) {  

        std::cout << "Program begins" << std::endl;

        while ( getline(maf_file, line) ) {
            
            read_count++;

            // line is 'a'

            // raw sequence
            getline(maf_file, str1);

            process( str1 );

            output << str1 << std::endl;

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
