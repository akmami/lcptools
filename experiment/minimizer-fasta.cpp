/**
 * @file    minimizer-fasta.cpp
 * @brief   Locating Minimizers in Genome Sequences
 *
 * This program reads the genome sequence and locates the minimizers in each window, using the
 * predefined k-mer and window sizes. Statistical data such as the total number of minimizers in the genome,
 * the number of unique minimizers, the mean of the distances between consecutive minimizers and their
 * standard deviation are calculated. Moreover, the execution time of locating the minimizers and also
 * the total size (GB) are given.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include "helper.cpp"

#define KMER_SIZE       19
#define WINDOW_SIZE     19
#define CAPACITY        250000000
#define COUNT_DISTINCT  true

using namespace std;

/**
 * @brief Struct to represent a minimizer
 *
 * This struct represents a minimizer, with members kmer (the encoded version of the kmer) and
 * position, which is the position of the kmer in the sequence. The less-than operator is
 * overloaded to facilitate the process of comparing the encoded kmer value of this kmer with
 * that of another kmer.
 *
 */
struct minimizer {
    kmer_type kmer;
    int position;
    
    minimizer(kmer_type kmer, int position) : kmer(kmer), position(position) {}

    bool operator<(const struct minimizer& other) const {
        return kmer < other.kmer;
    }
};

/**
 * @brief   Finds and inserts the lexicographically smallest k-mer into a minimizer vector.
 *
 * This function searches through a sequence of k-mers within the given range and
 * identifies the lexicographically smallest k-mer. It then encodes this k-mer and appends it
 * as a minimizer to the `minimizers` vector if it hasn't been added already.
 *
 * @param begin          Iterator pointing to the beginning of the sequence.
 * @param end            Iterator pointing to the end of the sequence.
 * @param current_index  The index of the first k-mer in the sequence.
 * @param kmerSize       The size of the k-mer (number of characters).
 * @param minimizers     A vector to store the resulting minimizers.
 * @param map            A pointer to an array used to encode k-mers.
 *
 */
void emplaceMinimizer(string::iterator begin, string::iterator end, int current_index, int kmerSize, vector <struct minimizer>& minimizers, int* map)
{
    string::iterator minimal_kmer = begin, current_kmer = begin + 1;
    int minimal_kmer_index = current_index, current_kmer_index = current_index, temp_index;
    
    while ( current_kmer < end )
    {
        temp_index = 0;

        while ( temp_index < kmerSize && *(current_kmer + temp_index) == *(minimal_kmer + temp_index) )
        {
            temp_index++;
        }

        if ( temp_index != kmerSize && *(current_kmer + temp_index) < *(minimal_kmer + temp_index) )
        {
            minimal_kmer = current_kmer;
            minimal_kmer_index = current_kmer_index;
        }

        current_kmer++;
        current_kmer_index++;
    }
    
    if (minimizers.empty() || minimizers.back().position != minimal_kmer_index )
    {
        struct minimizer minimizer( encode( map, minimal_kmer, minimal_kmer+kmerSize ), minimal_kmer_index );
        minimizers.push_back( minimizer );
    }
};

/**
 * @brief   Finds minimizers for each window in a genomic sequence and calculates processing time.
 *
 * This function scans through a given genomic sequence with windows of size `windowSize`.
 * For each window, it identifies the lexicographically smallest k-mer (minimizer) using the
 * `emplaceMinimizer` function and stores it in the `minimizers` vector. It also calculates the distances
 * between consecutive minimizers and records the time taken to process the sequence.
 *
 * @param sequence       The input sequence of nucleotides/characters to be processed.
 * @param minimizers     A vector to store the resulting minimizers.
 * @param kmerSize       The size of the k-mer (number of characters).
 * @param windowSize     The size of the sliding window in which the minimizers are searched.
 * @param map            A pointer to an array used for encoding k-mers.
 * @param distances      An array to store the frequencies of distances between consecutive minimizers.
 * @param processing_time A reference to the cumulative processing time.
 */
void findMinimizers(int& size, string& sequence, vector <struct minimizer>& minimizers, int kmerSize, int windowSize, int* map, int* distances, std::chrono::milliseconds& processing_time)
{
    int current_index = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    for ( string::iterator it = sequence.begin(); it < sequence.end() - windowSize - kmerSize; ++it )
    {
        emplaceMinimizer(it, it + windowSize, current_index, kmerSize, minimizers, map);
        current_index++;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    processing_time += std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time );
    size += minimizers.begin()->position;
    size += sequence.size() - 1 - ((minimizers.end() - 1)->position + kmerSize);
    for ( vector <struct minimizer>::iterator it = minimizers.begin()+1; it < minimizers.end(); it++ ) {
        distances[ it->position - (it - 1)->position ]++;
        if ( (it-1)->position + kmerSize < it->position) {
            size += it->position - ((it-1)->position + kmerSize);
        }
    }
    std::cout << "Length of the processed sequence: " << sequence.size() << std::endl;
};

// Read and process the genome sequence and print the results
int main(int argc, char** argv) {
    
    // Check if the correct number of arguments is provided
    if (argc < 2) {
        std::cerr << "Wrong format: " << argv[0] << " [infile]" << std::endl;
        return -1;
    }

    // Open the FASTA file using the filename from the command-line argument
    ifstream input(argv[1]);
    if (!input.good()) {
        std::cerr << "Error opening: " << argv[1] << ". You have failed." << std::endl;
        return -1;
    }

    // Variables
    std::string gen, line, id;
    std::chrono::milliseconds processing_time(0);
    int distances[WINDOW_SIZE + 1] = { 0 };
    int map[128];
    int gapSize = 0;

    gen.reserve(CAPACITY);
    init_map(map);

    vector<vector<struct minimizer>> minimizers;

    // Open genome file
    std::fstream genome;
    genome.open(argv[1], std::ios::in);
    
    // Read the file line by line
    if ( genome.is_open() ) {  
        
        std::cout << "Program begins" << std::endl;

        while ( getline(genome, line) ) {

            if (line[0] == '>') {

                // Process previous chromosome before moving into new one
                if (gen.size() != 0) {
                    vector <struct minimizer> sequence_minimizers;
                    sequence_minimizers.reserve( 3 * gen.size() / WINDOW_SIZE );
                    findMinimizers(gapSize, gen, sequence_minimizers, KMER_SIZE, WINDOW_SIZE, map, distances, processing_time);
                    minimizers.push_back(sequence_minimizers);
                }

                id = line.substr(1);
                std::cout << "Processing started for " << id << std::endl;

                // OMG!!
                gen.clear();

                continue;    
            }
            else if (line[0] != '>'){
                gen += line;
            }
        }
        
        // Process last chromosome before calculating stats
        if (gen.size() != 0) {
            vector <struct minimizer> sequence_minimizers;
            sequence_minimizers.reserve( 3 * gen.size() / WINDOW_SIZE );
            findMinimizers(gapSize, gen, sequence_minimizers, KMER_SIZE, WINDOW_SIZE, map, distances, processing_time);
            minimizers.push_back(sequence_minimizers);
            std::cout << "Found minimizers: " << sequence_minimizers.size() << std::endl; 
        }

        genome.close();
    }
    
    // Count total number of minimizers
    int numOfMinimizers = 0;
    for (size_t i = 0; i < minimizers.size(); i++)
    {
        numOfMinimizers += minimizers[i].size();   
    }

    // Count distinct minimizers
    int numOfDistintMinimizers = 0;
    if ( COUNT_DISTINCT ) {
        cout << "Counting distinct minimizers..." << endl;

        vector<kmer_type> flattened_minimizers;
        flattened_minimizers.reserve(numOfMinimizers);

        for (const auto& sequence_minimizers : minimizers) {
            for (const auto& minimizer : sequence_minimizers) {
                flattened_minimizers.push_back(minimizer.kmer);
            }
        }

        std::sort(flattened_minimizers.begin(), flattened_minimizers.end());

        numOfDistintMinimizers = 1;

        for ( vector<kmer_type>::iterator it = flattened_minimizers.begin() + 1; it < flattened_minimizers.end(); it++ ) {
            if ( *(it-1) != *(it) ) {
                numOfDistintMinimizers++;
            }
        }
    }

    cout << "Calculating stats..." << endl;
    
    // Calculate stats
    double average = mean(distances, WINDOW_SIZE);
    double std_dev = stdev(distances, WINDOW_SIZE, average);
    
    // Output the results
    cout << "K-mer size: " << KMER_SIZE << ", Window size: " << WINDOW_SIZE << endl;
    cout << "Total Minimizer: " << format_int(numOfMinimizers) << endl;
    COUNT_DISTINCT && cout << "Unique Minimizers: " << format_int(numOfDistintMinimizers) << endl;
    cout << "Exec. Time (sec): " << format_double( (((double) processing_time.count()) / 1000) ) << endl;
    cout << "Mean Minimizer Distances: " << format_double(average) << endl;
    cout << "Std Dev of Distances: " << format_double(std_dev) << endl;
    cout << "Gap size: " << gapSize << endl;
    cout << "Total Size (GB): " << format_double( (numOfMinimizers * sizeof(kmer_type)) / (1024.0 * 1024.0 * 1024.0)) << endl;
};
