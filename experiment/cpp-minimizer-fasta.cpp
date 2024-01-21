#include <iostream>
#include <cmath>
#include <chrono>
#include <string>
#include <vector>
#include "helper.cpp"


uint encode(std::string& read) {
    uint res = 0;
    for(std::string::iterator it = read.begin(); it != read.end(); it++) {
        res *= 4;
        res |= ( (*it) == 'A' ? 0 : ( (*it) == 'T' ? 3 : ( (*it) == 'C' ? 1 : 2 ) ) );
    }
    return res;
};

void analyze(std::vector<int> &minimizers, int (&distances)[DISTANCE_LENGTH], std::vector<int> &all_larger_distances_vec ) {

    for ( std::vector<int>::iterator it = minimizers.begin() + 1; it != minimizers.end(); it++ ) {
        if ( (*it) - (*(it-1)) < DISTANCE_LENGTH ) {
            distances[(*it) - (*(it-1))]++;
        } else {
            all_larger_distances_vec.push_back((*it) - (*(it-1)));
        }
    }
};

void find_minimizers(const std::string& sequence, uint kmer_size, uint window_size, std::vector<int>& minimizers) {
        
    if (sequence.length() < kmer_size || window_size < kmer_size) return;

    for ( uint i = window_size - kmer_size; i <= sequence.size() - window_size; i++ ) {
        std::string curr_kmer = sequence.substr(i, kmer_size);
        uint curr_val = encode(curr_kmer);
        bool isMinimizer = true;

        // Check if this k-mer is minimal in all positions in the window
        for ( uint j = i - window_size + kmer_size; j <= i + kmer_size; j++ ) {
            if ( j == i ) {
                continue;
            }
            std::string window_kmer = sequence.substr(j, kmer_size);
            uint win_val = encode(window_kmer);
            if (win_val <= curr_val) {
                isMinimizer = false;
                break;
            }
        }

        if (isMinimizer) {
            minimizers.push_back(i);
        }
    }
};

int main(int argc, char **argv) {

    if (argc != 4) {
        std::cerr << " Wrong format: " << argv[0] << " [infile] [kmer-size] [window-size]" << std::endl;
        return -1;  
    }

    std::ifstream genome(argv[1]);
    if (!genome.good()) {
        std::cerr << "Error opening: " << argv[1] << "" << std::endl;
        return -1;
    }

    uint kmer_size = atoi(argv[2]);
    uint window_size = atoi(argv[3]);

    // variables
    std::string line;

	// read file
    std::string sequence, id;
    int chrom_index = 0;
    sequence.reserve(250000000);
    std::chrono::milliseconds total_duration(0);
    size_t total_count = 0;
    int distances[DISTANCE_LENGTH] = {0};
    std::vector<int> all_larger_distances_vec;
    
    
    std::cout << "Program begins" << std::endl;

    while (getline(genome, line)) {

        if (line[0] == '>') {

            chrom_index++;

            // process previous chromosome before moving into new one
            if (sequence.size() != 0) {
                
                auto start = std::chrono::high_resolution_clock::now();

                std::vector<int> minimizers;
                
                find_minimizers(sequence, kmer_size, window_size, minimizers);

                auto end = std::chrono::high_resolution_clock::now();

                total_duration += std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
                total_count += minimizers.size();

                analyze(minimizers, distances, all_larger_distances_vec);

                std::cout << "Processing is done for " << id << std::endl;
                std::cout << "Length of the processed sequence: " << sequence.size() << std::endl;
                std::cout << std::endl;

                sequence.clear();
            }

            if ( chrom_index > 23 ) {
                break;
            }
            
            id = line.substr(1);
            std::cout << "Processing started for " << id << std::endl;
            
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
