/**
 * @file    compare-gfa.cpp
 * @brief   Genomic Link Comparison in GFA Files
 *
 * This program is designed to compare links between genomic reads in GFA (Graphical Fragment Assembly)
 * files. It reads two GFA files - a gold-standard file and another file to compare - and evaluates the
 * links between reads. The program determines the correctness of the links in the comparison file by
 * checking them against the gold-standard file. It also supports the option to consider reverse-direction
 * links as equivalent (bidirectional comparison).
 *
 * The comparison results include the count of correct and incorrect links and the total number of unique
 * links in the gold-standard file, providing insights into the accuracy of the genomic assembly represented
 * in the compared GFA file.
 */


#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>

const bool CONSIDER_REV = true;


/**
 * @brief Parses a line from a GFA file to extract and store links.
 *
 * This function reads a line from a GFA file, representing a link between two genomic reads, 
 * and stores it in a set. If reverse-direction consideration is enabled, it also stores the 
 * reverse link.
 *
 * @param line The line from the GFA file representing a link.
 * @param links Set to store extracted links.
 * @param considerRev Boolean flag to consider reverse-direction links as equivalent.
 */
void parse_link(const std::string& line, std::unordered_set<std::string>& links, bool considerRev) {
    std::istringstream iss(line);
    std::string segment, read1, read2;

    // skip the first segment 'L'
    iss >> segment;

    // get read1 and read2
    if (iss >> read1 >> segment >> read2) {
        links.insert(read1 + "-" + read2);

        if (considerRev) {
            links.insert(read2 + "-" + read1);
        }
    }
}


/**
 * @brief Reads a GFA file and extracts all links.
 *
 * This function opens and reads a GFA file, extracting the links between genomic reads. It utilizes
 * the parse_link function to process each line that represents a link. The function returns a set
 * of all extracted links.
 *
 * @param filename The name of the GFA file to read.
 * @param considerRev Boolean flag to consider reverse-direction links as equivalent.
 * @return A set of strings, each representing a link from the GFA file.
 */
std::unordered_set<std::string> read_links(const std::string& filename, bool considerRev) {
    std::unordered_set<std::string> links;
    std::ifstream infile(filename);
    std::string line;

    if (!infile.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return links;
    }

    while (getline(infile, line)) {
        if (line[0] == 'L') {
            parse_link(line, links, considerRev);
        }
    }

    return links;
}


/**
 * @brief The entry point of the program for comparing genomic links in GFA files.
 *
 * The main function orchestrates the comparison of genomic links between two GFA files.
 * It reads and processes the gold-standard and another GFA file, compares their links,
 * and outputs the number of correct and incorrect links based on the gold-standard. It also
 * provides the total count of unique links in the gold-standard file.
 *
 * Usage: <ExecutableName> <GoldStandardGFAFile.gfa> <OtherGFAFile.gfa>
 */
int main(int argc, char** argv) {

    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [gold-standard] [other-gfa]" << std::endl;
        return -1;  
    }

    std::string gold(argv[1]);
    std::string other(argv[2]);

    std::unordered_set<std::string> gold_links = read_links(gold, CONSIDER_REV);
    std::unordered_set<std::string> other_links = read_links(other, CONSIDER_REV);

    int correct = 0;
    int incorrect = 0;

    for (std::unordered_set<std::string>::iterator link = other_links.begin(); link != other_links.end(); link++) {
        if ( gold_links.find(*link) != gold_links.end() ) {
            correct++;
        } else {
            incorrect++;
        }
    }

    std::cout << "Correct links:    " << correct << std::endl;
    std::cout << "Incorrect links:  " << incorrect << std::endl;

    if ( CONSIDER_REV ) {
        std::cout << "Total:            " << gold_links.size() / 2 << std::endl;
    } else {
        std::cout << "Total:            " << gold_links.size() << std::endl;
    }
    

    return 0;
}
