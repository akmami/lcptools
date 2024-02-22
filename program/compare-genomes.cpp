/**
 * @file    GenomeAnalysis.cpp
 * @brief   Multithreaded Genomic Data Analysis
 *
 * This program performs multithreaded processing of genomic data from two input files,
 * constructing an inverted index for each genome at a specified LCP level. It efficiently
 * processes genomic sequences to extract and deepen LCP structures, facilitating the
 * analysis of genomic features. The program computes Jaccard and Dice similarity metrics
 * between the two genomes based on the features extracted and indexed. Utilizing
 * multithreading, the program aims to enhance performance and manage large-scale genomic
 * data analysis tasks.
 *
 * Dependencies: Requires the GzFile, InvertedIndex, and LCP classes for handling gzip-compressed
 * files, and processing LCP structures, respectively.
 *
 * Usage:
 *     ./GenomeAnalysis genome-input-1.gz genome-input-2.gz lcp-level
 */


#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>
#include "../string.cpp"
#include "../utils/process.cpp"
#include "../utils/GzFile.hpp"
#include "../utils/ThreadSafeQueue.hpp"
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <condition_variable>

#define     THREAD_NUMBER           4
#define     MERGE_CORE_THRESHOLD    200

std::mutex results_mutex; // mutex for protecting access to the results vector


struct Task {
    std::string read;
};


// Worker function that processes tasks and merges results periodically
void worker_function(ThreadSafeQueue<Task>& task_queue, std::vector<uint>& lcp_cores, int lcp_level) {
    std::vector<uint> local_cores;
    Task task;

    auto merge_results = [&]() {
        std::lock_guard<std::mutex> lock(results_mutex);
        lcp_cores.insert(lcp_cores.end(), local_cores.begin(), local_cores.end());
        local_cores.clear();
    };

    while ( task_queue.pop(task) || !task_queue.isFinished() ) {

        lcp::string *lcp = new lcp::string(task.read);
        lcp->deepen(lcp_level);
        
        for ( std::vector<lcp::core*>::iterator it = lcp->cores.begin(); it != lcp->cores.end(); it++ ) {
            local_cores.push_back( (*it)->label );
        }

        delete lcp;
        lcp = NULL;

        reverseComplement(task.read);

        lcp = new lcp::string(task.read);
        lcp->deepen(lcp_level);
        
        for ( std::vector<lcp::core*>::iterator it = lcp->cores.begin(); it != lcp->cores.end(); it++ ) {
            local_cores.push_back( (*it)->label );
        }

        delete lcp;

        // periodically merge results to the main vector to reduce locking overhead
        if (local_cores.size() >= MERGE_CORE_THRESHOLD) {
            merge_results();
        }
    }

    // merge any remaining results after all tasks are processed
    if (!local_cores.empty()) {
        merge_results();
    }
};


void generate_minhash_signature( std::vector<uint>& hash_values ) {
    std::sort(hash_values.begin(), hash_values.end());
};


/**
 * @brief Calculates the intersection and union sizes between two sets of hashed LCP cores.
 *
 * Given two vectors containing hashed LCP cores from two genomes, this function calculates
 * the sizes of their intersection and union. These sizes are essential for computing
 * similarity metrics between the genomes.
 *
 * @param hashed_cores1 A vector containing the hashed LCP cores of the first genome.
 * @param hashed_cores2 A vector containing the hashed LCP cores of the second genome.
 * @param intersection_size A reference to a size_t variable to store the intersection size.
 * @param union_size A reference to a size_t variable to store the union size.
 */
void calculate_intersection_and_union_sizes( std::vector<uint>& hashed_cores1, std::vector<uint>& hashed_cores2, size_t& intersection_size, size_t& union_size ) {

    std::vector<uint>::iterator it1 = hashed_cores1.begin(), it2 = hashed_cores2.begin();
    
    while ( it1 != hashed_cores1.end() && it2 != hashed_cores2.end() ) {
        if ( *it1 < *it2 ) {
            it1++;
        } else if ( *it1 > *it2 ) {
            it2++;
        } else {
            intersection_size++;
            it1++;
            it2++;
        }
        union_size++;
    }

    // count the remaining elements in either vector
    while ( it1 != hashed_cores1.end() ) {
        it1++;
        union_size++;
    }
    while ( it2 != hashed_cores2.end() ) {
        it2++;
        union_size++;
    }
};


/**
 * @brief Calculates the Jaccard similarity between two genomes.
 *
 * Computes the Jaccard similarity metric based on the intersection and union sizes
 * of hashed LCP cores from two genomes. This metric provides a measure of similarity
 * in terms of shared genomic features.
 *
 * @param intersection_size The size of the intersection between the two sets of cores.
 * @param union_size The size of the union between the two sets of cores.
 * @return The Jaccard similarity coefficient as a double.
 */
double calculate_jaccard_similarity( size_t intersection_size, size_t union_size ) {
    return static_cast<double>(intersection_size) / static_cast<double>(union_size);
};


/**
 * @brief Calculates the Dice similarity between two genomes.
 *
 * Computes the Dice similarity metric based on the intersection size of hashed LCP cores
 * and the sizes of individual core sets from two genomes. This metric is another measure
 * of similarity focusing on shared genomic features.
 *
 * @param intersection_size The size of the intersection between the two sets of cores.
 * @param first_size The size of the first set of cores.
 * @param second_size The size of the second set of cores.
 * @return The Dice similarity coefficient as a double.
 */
double calculate_dice_similarity( size_t intersection_size, size_t first_size, size_t second_size ) {
    return 2 * static_cast<double>(intersection_size) / ( static_cast<double>(first_size) + static_cast<double>(second_size) );
};


/**
 * @brief Reads and processes genomic sequences from a file, extracting LCP cores.
 *
 * This function operates in a multithreaded environment to process genomic sequences
 * from a gzip-compressed file. For each sequence, it extracts LCP cores at a specified
 * depth and stores these cores for later analysis. The function supports parallel
 * processing of two genomes to enhance performance for large-scale genomic datasets.
 *
 * @param filename Path to the gzip-compressed input file containing genomic sequences.
 * @param infile Reference to an open GzFile object for reading the input file.
 * @param lcp_level The depth for LCP analysis to extract cores.
 * @param lcp_cores Reference to a vector to store the extracted LCP cores.
 * @param read_count Reference to a size_t variable to track the number of processed reads.
 */
void process_genome(const char* filename, GzFile& infile, int lcp_level, std::vector<uint>& lcp_cores, size_t& read_count) {
    
    ThreadSafeQueue<Task> task_queue;
    std::vector<std::thread> workers;

    // start worker threads
    for (int i = 0; i < THREAD_NUMBER; ++i) {
        workers.emplace_back(worker_function, std::ref(task_queue), std::ref(lcp_cores), lcp_level);
    }

    std::cout << "Processing is started for " << filename << std::endl;
    
    // variables
    char buffer[BUFFERSIZE];

    // read file
    std::string sequence;
        
    sequence.reserve(BUFFERSIZE);

    while (true) {
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        infile.gets(buffer, BUFFERSIZE);
        sequence.append(buffer);

        // assign read to tasks queue
        Task task;
        task.read = std::string(buffer);
        task_queue.push(task);

        read_count++;

        infile.gets(buffer, BUFFERSIZE);
        infile.gets(buffer, BUFFERSIZE);
    }

    task_queue.markFinished();

    // wait for all worker threads to complete
    for (std::vector<std::thread>::iterator it = workers.begin(); it != workers.end(); it++ ) {
        if ((*it).joinable()) {
            (*it).join();
        }
    }

    generate_minhash_signature(lcp_cores);

};


/**
 * @brief Main entry point for genomic similarity analysis.
 *
 * Orchestrates the parallel processing of two genomic data files to extract LCP cores,
 * computes minhash signatures for these cores (if applicable), and then calculates
 * similarity metrics between the genomes. Outputs the computed Jaccard and Dice
 * similarity metrics, providing a quantitative measure of genomic similarity.
 *
 * Usage:
 *     ./GenomeAnalysis genome-input-1.gz genome-input-2.gz lcp-level
 *
 * Where genome-input-1.gz and genome-input-2.gz are the gzip-compressed files containing
 * the genomic sequences to compare, and lcp-level specifies the depth of LCP analysis.
 */
int main(int argc, char **argv) {

    if (argc != 4) {
        std::cerr << "Wrong format: " << argv[0] << " [genome-input-1] [genome-input-2] [lcp-level]" << std::endl;
        return -1;  
    }

    // validate input file 1
    GzFile infile1( argv[1], "rb");
    if ( !infile1 ) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // validate input file 2
    GzFile infile2( argv[2], "rb");
    if ( !infile2 ) {
        std::cerr << "Failed to open file: " << argv[2] << std::endl;
        return 1;
    }

    int lcp_level = atoi(argv[3]);

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    std::vector<uint> lcp_cores_1, lcp_cores_2;
    lcp_cores_1.reserve(1000000000);
    lcp_cores_2.reserve(1000000000);

    size_t read_count_1 = 0, read_count_2 = 0;

    // create threads to process each genome in parallel
    std::thread thread1(process_genome, argv[1], std::ref(infile1), lcp_level, std::ref(lcp_cores_1), std::ref(read_count_1));
    std::thread thread2(process_genome, argv[2], std::ref(infile2), lcp_level, std::ref(lcp_cores_2), std::ref(read_count_2));

    // wait for both threads to finish
    thread1.join();
    thread2.join();
    
    std::cout << "Processing is done for " << argv[1] << std::endl;
    std::cout << "Total number of processed reads count: " << read_count_1 << std::endl;
    std::cout << "Total number of cores: " << lcp_cores_1.size() << std::endl;

    std::cout << "Processing is done for " << argv[2] << std::endl;
    std::cout << "Total number of processed reads count: " << read_count_2 << std::endl;
    std::cout << "Total number of cores: " << lcp_cores_2.size() << std::endl; 

    size_t intersection_size = 0, union_size = 0;

    calculate_intersection_and_union_sizes(lcp_cores_1, lcp_cores_2, intersection_size, union_size);

    double jaccard_similarity = calculate_jaccard_similarity(intersection_size, union_size);
    double dice_similarity = calculate_dice_similarity(intersection_size, lcp_cores_1.size(), lcp_cores_2.size());

    std::cout << "Intersection Size: " << intersection_size << std::endl;
    std::cout << "Union Size: " << union_size << std::endl;
    std::cout << std::endl;
    std::cout << "Jaccard Similarity: " << jaccard_similarity << std::endl;
    std::cout << "Dice Similarity: " << dice_similarity << std::endl;

    return 0;
};
