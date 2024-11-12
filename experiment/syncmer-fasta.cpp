/**
 * @file    syncmer-fasta.cpp
 * @brief   Locating Syncmers in Genome Sequences
 *
 * This program reads the genome sequences and locates the syncmers in it by
 * distinguishing the kmers that have their lexicographically smallest smers starting/
 * ending in the chosen indices. Statistical data such as the total number of syncmers,
 * the number of unique syncmers, the mean of the distances between consecutive
 * syncmers and their standard deviation are calculated. Additionally, the execution time
 * of locating the syncmers and also the total size (GB) are given.
 *
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include "helper.cpp"

#define KMER_SIZE           15
#define SMER_SIZE           4
#define SMER_BEGIN_INDEX    0
#define CAPACITY            250000000
#define DISTANCE_ARRAY_SIZE 9999
#define COUNT_DISTINCT      true

using namespace std;

/**
 * @brief Struct to represent a syncmer
 *
 * This struct represents a syncmer, with members kmer (the encoded version of the kmer)
 * and position, which is the position of the kmer in the sequence. The less-than operator is
 * overloaded to facilitate the process of comparing the encoded kmer value of this kmer with
 * that of another kmer.
 */
struct syncmer {
    kmer_type kmer;
    int position;

    syncmer(kmer_type kmer, int position) : kmer(kmer), position(position) {}

    bool operator<(const struct syncmer& other) const {
        return kmer < other.kmer;
    }
};

/**
 * @brief   Identifies and inserts a syncmer into the syncmer vector if it meets certain conditions.
 *
 * This function scans through a k-mer to identify the lexicographically smallest s-mer (substring of size `smerSize`)
 * and compares its index with predefined indicex (`beginIndex`). If the smallest s-mer is found
 * at either of these positions, the corresponding syncmer is encoded and appended to the `syncmers` vector.
 *
 * @param beginIndex     The index to compare with the smallest s-mer's position.
 * @param begin          Iterator pointing to the beginning of the current k-mer.
 * @param end            Iterator pointing to the end of the current k-mer.
 * @param current_index  The index of the current k-mer in the sequence.
 * @param smerSize       The size of the s-mer (number of characters).
 * @param kmerSize       The size of the k-mer (number of characters).
 * @param syncmers       A vector to store the resulting syncmers.
 * @param map            A pointer to an array used to encode s-mers.
 */
void emplaceSyncmer(int beginIndex, string::iterator begin, string::iterator end, int current_index, int smerSize, int kmerSize, vector <struct syncmer>& syncmers, int* map)
{
    string::iterator minimal_smer_it = begin, current_smer_it = begin + 1;
    int minimal_smer_index = 0, temp_index;
    
    while ( current_smer_it + smerSize <= end )
    {   
        temp_index = 0;

        while ( temp_index < smerSize && ( *(minimal_smer_it + temp_index) | 0b100000 ) == ( *(current_smer_it + temp_index) | 0b100000 ) ) 
        {
            temp_index++;
        }

        if ( temp_index != smerSize && ( *(minimal_smer_it + temp_index) | 0b100000 ) > ( *(current_smer_it + temp_index) | 0b100000 ) )
        {   
            minimal_smer_it = current_smer_it;
            minimal_smer_index = current_smer_it - begin;
        }

        current_smer_it++;
    }
    
    if (minimal_smer_index == beginIndex)
    {
        struct syncmer syncmer( encode( map, begin, begin + kmerSize ), current_index );
        syncmers.push_back( syncmer );
    }
};

/**
 * @brief   Finds syncmers in a genomic sequence and calculates processing time.
 *
 * This function processes a genomic sequence by sliding a window of size `kmerSize` across it, calling
 * `emplaceSyncmer` to identify syncmers based on predefined index (`beginIndex`). The
 * function also calculates distances between consecutive syncmers and measures the time taken to process
 * the entire sequence.
 *
 * @param beginIndex     The  index to compare with the smallest s-mer's position.
 * @param sequence       The input sequence of nucleotides/characters to be processed.
 * @param syncmers       A vector to store the resulting syncmers.
 * @param kmerSize       The size of the k-mer (number of characters).
 * @param smerSize       The size of the s-mer (number of characters).
 * @param map            A pointer to an array used for encoding k-mers.
 * @param distances      An array to store the frequencies of distances between consecutive syncmers.
 * @param processing_time A reference to the cumulative processing time.
 */
void findSyncmers(int& size, int beginIndex, string& sequence, vector <struct syncmer>& syncmers, int kmerSize, int smerSize, int* map, int* distances, std::chrono::milliseconds& processing_time)
{
    int current_index = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    for ( string::iterator it = sequence.begin(); it <= sequence.end() - kmerSize; ++it )
    {
        emplaceSyncmer(beginIndex, it, it + kmerSize, current_index, smerSize, kmerSize, syncmers, map);
        current_index++;
    }

    auto end_time = std::chrono::high_resolution_clock::now();

    processing_time += std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time );

    size += syncmers.begin()->position;
    size += sequence.size() - 1 - ((syncmers.end() - 1)->position + kmerSize);
    for ( vector <struct syncmer>::iterator it = syncmers.begin()+1; it < syncmers.end(); it++ ) {
        distances[ it->position - (it - 1)->position ]++;
        if ( (it-1)->position + kmerSize < it->position) {
            size += it->position - ((it-1)->position + kmerSize);
        }
    }

    std::cout << "Length of the processed sequence: " << sequence.size() << std::endl;
};

// Read and process the sequence and print the results
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
    int distances[DISTANCE_ARRAY_SIZE] = { 0 };
    int map[128];
    int gapSize = 0;

    gen.reserve(CAPACITY);
    init_map(map);
    
    vector<vector <struct syncmer>> syncmers;

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
                    vector<struct syncmer> sequence_syncmers;
                    sequence_syncmers.reserve( gen.size() );
                    findSyncmers(gapSize, SMER_BEGIN_INDEX, gen, sequence_syncmers, KMER_SIZE, SMER_SIZE, map, distances, processing_time);
                    syncmers.push_back(sequence_syncmers);
                    std::cout << "Found syncmers: " << sequence_syncmers.size() << std::endl; 
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
            vector <struct syncmer> sequence_syncmers;
            sequence_syncmers.reserve( gen.size() );
            findSyncmers(gapSize, SMER_BEGIN_INDEX, gen, sequence_syncmers, KMER_SIZE, SMER_SIZE, map, distances, processing_time);
            syncmers.push_back(sequence_syncmers);
            std::cout << "Found syncmers: " << sequence_syncmers.size() << std::endl;
        }

        genome.close();
    }
    
    // Count total number of Sycnmers
    int numOfSyncmers = 0;
    for (size_t i = 0; i < syncmers.size(); i++)
    {
        numOfSyncmers += syncmers[i].size();   
    }

    // Count distinct syncmers
    int numOfDistintSyncmers = 0;
    if ( COUNT_DISTINCT ) {
        cout << "Counting distinct syncmers..." << endl;

        vector<kmer_type> flattened_syncmers;
        flattened_syncmers.reserve(numOfSyncmers);

        for (const auto& sequence_syncmers : syncmers) {
            for (const auto& syncmer : sequence_syncmers) {
                flattened_syncmers.push_back(syncmer.kmer);
            }
        }

        std::sort(flattened_syncmers.begin(), flattened_syncmers.end());

        numOfDistintSyncmers = 1;

        for ( vector<kmer_type>::iterator it = flattened_syncmers.begin() + 1; it < flattened_syncmers.end(); it++ ) {
            if ( *(it-1) != *(it) ) {
                numOfDistintSyncmers++;
            }
        }
    }
    
    cout << "Calculating stats..." << endl;

    // Calculate stats
    double average = mean(distances, DISTANCE_ARRAY_SIZE);
    double std_dev = stdev(distances, DISTANCE_ARRAY_SIZE, average);

    // Output the results
    cout << "K-mer size: " << KMER_SIZE << ", S-mer size: " << SMER_SIZE << endl;
    cout << "Total Syncmer: " << format_int(numOfSyncmers) << endl;
    COUNT_DISTINCT && cout << "Unique Syncmers: " << format_int(numOfDistintSyncmers) << endl;
    cout << "Exec. Time (sec): " << format_double( (((double) processing_time.count()) / 1000) ) << endl;
    cout << "Mean Syncmer Distances: " << format_double(average) << endl;
    cout << "Std Dev of Distances: " << format_double(std_dev) << endl;
    cout << "Gap size: " << format_double(gapSize) << endl;
    cout << "Total Size (GB): " << format_double( (numOfSyncmers * sizeof(kmer_type)) / (1024.0 * 1024.0 * 1024.0)) << endl;
};