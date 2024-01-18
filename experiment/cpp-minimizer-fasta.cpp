#include <iostream>
#include <cmath>
#include <chrono>
#include <string>
#include <vector>
#include "helper.cpp"



void analyze(std::vector<std::pair<std::string, int>> &minimizers, int (&distances)[DISTANCE_LENGTH], std::vector<int> &all_larger_distances_vec ) {

    std::vector<std::pair<std::string, int>>::iterator it = minimizers.begin() + 1;

    for ( ; it != minimizers.end(); it++ ) {
        if ( (*it).second - (*(it-1)).second < DISTANCE_LENGTH ) {
            distances[(*it).second - (*(it-1)).second]++;
        } else {
            all_larger_distances_vec.push_back((*it).second - (*(it-1)).second);
        }
    }
};

std::vector<std::pair<std::string, int>> find_minimizers(const std::string& sequence) {
    
    std::vector<std::pair<std::string, int>> minimizers;
    
    if (sequence.length() < KMER_SIZE || WINDOW_SIZE < KMER_SIZE) return minimizers;

    for (int i = 0; i <= sequence.length() - KMER_SIZE; ++i) {
        std::string curr_kmer = sequence.substr(i, KMER_SIZE);
        bool isMinimizer = true;

        // Check if this k-mer is minimal in all positions in the window
        for (int j = std::max(0, i - WINDOW_SIZE + 1); j <= std::min(i + WINDOW_SIZE - KMER_SIZE, static_cast<int>(sequence.length()) - KMER_SIZE); ++j) {
            std::string window_kmer = sequence.substr(j, KMER_SIZE);
            if (window_kmer < curr_kmer) {
                isMinimizer = false;
                break;
            }
        }

        if (isMinimizer) {
            minimizers.push_back(std::make_pair(curr_kmer, i));
        }
    }

    return minimizers;
};

int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << " Wrong format: " << argv[0] << " [infile] " << std::endl;
        return -1;  
    }

    std::ifstream genome(argv[1]);
    if (!genome.good()) {
        std::cerr << "Error opening: " << argv[1] << "" << std::endl;
        return -1;
    }

    // variables
    std::string line;

	// read file
    std::string sequence, id;
    int chrom_index = 0;
    sequence.reserve(250000000);
    std::chrono::milliseconds total_duration(0);
    int total_count = 0;
    int distances[DISTANCE_LENGTH] = {0};
    std::vector<int> all_larger_distances_vec;
    
    
    std::cout << "Program begins" << std::endl;

    while (getline(genome, line)) {

        if (line[0] == '>') {

            chrom_index++;

            // process previous chromosome before moving into new one
            if (sequence.size() != 0) {
                
                auto start = std::chrono::high_resolution_clock::now();

                std::vector<std::pair<std::string, int>> minimizers = find_minimizers(sequence);

                auto end = std::chrono::high_resolution_clock::now();

                total_duration += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
                total_count += minimizers.size();

                analyze(minimizers, distances, all_larger_distances_vec);
            }

            if ( chrom_index > 23 ) {
                break;
            }
            
            id = line.substr(1);
            std::cout << "Processing started for " << id << std::endl;
            std::cout << std::endl;
            
            continue;   
            
        }
        else if (line[0] != '>'){
            sequence += line;
        }
    }

    genome.close();

    summaryMimimizer(distances, all_larger_distances_vec, total_duration, total_count);

    return 0;
};
