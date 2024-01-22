#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <sstream>

std::string parse_link(const std::string& line, std::unordered_set<std::string>& links, bool considerRev) {
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

    return "";
}

std::unordered_set<std::string> read_links(const std::string& filename) {
    std::unordered_set<std::string> links;
    std::ifstream infile(filename);
    std::string line;

    if (!infile.is_open()) {
        std::cerr << "Unable to open file: " << filename << std::endl;
        return links;
    }

    while (getline(infile, line)) {
        if (line[0] == 'L') {
            std::string link = parse_link(line);
            if (!link.empty()) {
                insert(link, links, true);
            }
        }
    }

    return links;
}

int main(int argc, char** argv) {

    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [gold-standard] [other-gfa]" << std::endl;
        return -1;  
    }

    std::string gold(argv[1]);
    std::string other(argv[2]);

    std::unordered_set<std::string> gold_links = read_links(gold);
    std::unordered_set<std::string> other_links = read_links(other);

    int correct = 0;
    int incorrect = 0;

    for (std::unordered_set<std::string>::iterator link = other_links.begin(); link != other_links.end(); link++) {
        if ( gold_links.find(*link) != gold_links.end() ) {
            correct++;
        } else {
            incorrect++;
        }
    }

    std::cout << "Correct links: " << correct << std::endl;
    std::cout << "Incorrect links: " << incorrect << std::endl;

    return 0;
}
