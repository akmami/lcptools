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
 *     ./GenomeAnalysis genome-input-1.gz genome-input-2.gz -t lcp-level -t 8 --genome-1-len 3088269832 -genome-2-len 3088269832
 */


#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>
#include "../string.cpp"
#include "../utils/process.cpp"
#include "../utils/GzFile.hpp"
#include "../utils/ThreadSafeQueue.hpp"
#include "../utils/argp.hpp"
#include <stdexcept>
#include <mutex>


#define     THREAD_NUMBER           8
#define     MERGE_CORE_THRESHOLD    200


std::mutex results_mutex; // mutex for protecting access to the results vector


struct Task {
    std::string read;
};


/**
 * @brief Processes genomic reads from a queue, extracts LCP cores, and aggregates results in a shared vector.
 *
 * This function runs in a worker thread and is responsible for processing genomic reads
 * retrieved from a thread-safe queue. For each read, it computes the LCP cores at a specified
 * LCP level, processes the reverse complement of the read, computes its LCP cores, and then
 * aggregates these cores in a shared vector. The function ensures thread-safe access to the
 * shared vector using mutexes and minimizes locking overhead by merging local results into
 * the shared vector periodically.
 *
 * @param task_queue The thread-safe queue from which tasks (genomic reads) are retrieved.
 * @param lcp_cores A shared vector to store the labels of LCP cores extracted from the reads.
 * @param lcp_level The depth of analysis for extracting LCP cores from the reads.
 */
void processRead( ThreadSafeQueue<Task>& task_queue, std::vector<uint>& lcp_cores, int lcp_level ) {
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


/**
 * @brief Populates vectors with unique LCP cores and their counts from a sorted vector of LCP cores.
 *
 * This function analyzes a sorted vector of LCP cores, identifying each unique core and
 * counting the number of occurrences of that core within the vector. The unique cores
 * and their counts are then stored in separate vectors. This operation is useful for
 * summarizing the distribution of LCP cores within a dataset, particularly in genomic
 * data analysis where understanding the frequency of certain sequences or patterns can
 * be critical.
 *
 * @param lcp_cores The input vector containing sorted LCP cores to be analyzed.
 * @param unique_lcp_cores An output vector that will contain all unique LCP cores
 *                         identified in the input vector.
 * @param lcp_cores_count An output vector that will contain the counts of each unique
 *                        LCP core corresponding to the unique cores in `unique_lcp_cores`.
 */
void fillValuesAndCounts( std::vector<uint>& lcp_cores, std::vector<size_t>& unique_lcp_cores, std::vector<size_t>& lcp_cores_count ) {
    
    if( lcp_cores.empty() ) {
        return;
    }

    int unique_lcp_core_count = 1;

    for( size_t i = 1; i < lcp_cores.size(); i++ ) {
        if( lcp_cores[i] != lcp_cores[i - 1] ) {
            unique_lcp_core_count++;
        }
    }
    
    // pre-allocate memory for efficiency
    unique_lcp_cores.reserve( unique_lcp_core_count ); 
    lcp_cores_count.reserve( unique_lcp_core_count );
    
    unique_lcp_cores.push_back(lcp_cores[0]);
    size_t currentCount = 1;
    
    for (size_t i = 1; i < lcp_cores.size(); ++i) {
        if (lcp_cores[i] == lcp_cores[i - 1]) {
            currentCount++;
        } else {
            lcp_cores_count.push_back(currentCount);
            unique_lcp_cores.push_back(lcp_cores[i]);
            currentCount = 1;
        }
    }

    // add the count for the last element
    lcp_cores_count.push_back(currentCount); 
};


void generateMinhashSignature( std::vector<uint>& hash_values ) {
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
void calculateIntersectionAndUnionSizes( std::vector<uint>& hashed_cores1, std::vector<uint>& hashed_cores2, size_t& intersection_size, size_t& union_size ) {

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
double calculateJaccardSimilarity( size_t intersection_size, size_t union_size ) {
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
double calculateDiceSimilarity( size_t intersection_size, size_t first_size, size_t second_size ) {
    return 2 * static_cast<double>(intersection_size) / ( static_cast<double>(first_size) + static_cast<double>(second_size) );
};


/**
 * @brief Calculates a similarity score between two sets of hashed LCP cores.
 *
 * This function compares two sets of hashed LCP cores, representing genomic sequences
 * analyzed at specified depths, to compute a similarity score. The score is based on
 * the normalized Manhattan distance between the weighted counts of matching LCP cores,
 * adjusted to reflect similarity. This approach considers both the presence and abundance
 * of LCP cores in relation to the analysis depths, offering a nuanced similarity measure
 * suitable for genomic comparisons.
 *
 * @param hashed_cores1 First set of hashed LCP cores.
 * @param hashed_cores2 Second set of hashed LCP cores.
 * @param depth_1 Depth of analysis for the first set of cores.
 * @param depth_2 Depth of analysis for the second set of cores.
 * @param same_count Reference to store the count of matching cores across both sets.
 * @return A double representing the similarity score, ranging from 0 (no similarity) to 1 (identical).
 */
double calculateDistanceSimilarity( std::vector<uint>& hashed_cores1, std::vector<uint>& hashed_cores2, double depth_1, double depth_2, size_t& same_count ) {

    std::vector<size_t> unique_lcp_cores_1, unique_lcp_cores_2;
    std::vector<size_t> lcp_cores_count_1, lcp_cores_count_2;

    fillValuesAndCounts(hashed_cores1, unique_lcp_cores_1, lcp_cores_count_1);
    fillValuesAndCounts(hashed_cores2, unique_lcp_cores_2, lcp_cores_count_2);
    
    double numerator = 0.0;
    double denominator = 0.0;
    size_t i = 0, j = 0;

    same_count = 0;

    int diff = 0;

    while( i < unique_lcp_cores_1.size() && j < unique_lcp_cores_2.size() ) {
        if( unique_lcp_cores_1[i] == unique_lcp_cores_2[j] ) {
            diff = lcp_cores_count_1[i] * depth_2 - lcp_cores_count_2[j] * depth_1;
            numerator += diff >= 0 ? diff : -diff;
            denominator += (lcp_cores_count_1[i] * depth_2 + lcp_cores_count_2[j] * depth_1);
            same_count += lcp_cores_count_1[i] < lcp_cores_count_2[j] ? lcp_cores_count_1[i] : lcp_cores_count_2[j];
            i++;
            j++;
        } else if (unique_lcp_cores_1[i] < unique_lcp_cores_2[j]) {
            numerator += lcp_cores_count_1[i] * depth_2;
            denominator += lcp_cores_count_1[i] * depth_2;
            i++;
        } else {
            numerator += lcp_cores_count_2[j] * depth_1;
            denominator += lcp_cores_count_2[j] * depth_1;
            j++;
        }
    }

    while( i < unique_lcp_cores_1.size() ) {
        numerator += lcp_cores_count_1[i] * depth_2;
        denominator += lcp_cores_count_1[i] * depth_2;
        i++;
    }

    while( j < unique_lcp_cores_2.size() ) {
        numerator += lcp_cores_count_2[j] * depth_1;
        denominator += lcp_cores_count_2[j] * depth_1;
        j++;
    }

    // check for division by zero before returning the result
    return denominator != 0.0 ? 1 - numerator / denominator : 0.0;
};


/**
 * @brief Processes a genome file to extract LCP cores using multiple threads.
 *
 * This function reads genomic sequences from a specified file and distributes the processing
 * tasks among several worker threads. Each thread computes LCP cores for the sequences at a
 * given LCP level and aggregates these cores into a shared vector. The function tracks the
 * total number of reads processed and their combined length. It ensures efficient and
 * thread-safe handling of genomic data, leveraging parallel processing to enhance performance.
 *
 * @param filename Path to the input file containing genomic sequences.
 * @param infile Reference to an open GzFile object for reading the input file.
 * @param lcp_level The depth of LCP analysis for extracting cores from sequences.
 * @param lcp_cores A reference to a shared vector where extracted LCP cores are aggregated.
 * @param read_count Reference to a variable that will hold the total number of processed reads.
 * @param total_length Reference to a variable that will hold the combined length of all reads.
 * @param thread_number The number of worker threads to use for processing.
 */
void processGenome( const char* filename, GzFile& infile, int lcp_level, std::vector<uint>& lcp_cores, size_t& read_count, size_t& total_length, size_t thread_number ) {
    
    read_count = 0;
    total_length = 0;

    ThreadSafeQueue<Task> task_queue;
    std::vector<std::thread> workers;

    // start worker threads
    for (size_t i = 0; i < thread_number; ++i) {
        workers.emplace_back(processRead, std::ref(task_queue), std::ref(lcp_cores), lcp_level);
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

        while ( !task_queue.isAvailable() );

        // assign read to tasks queue
        Task task;
        task.read = std::string(buffer);
        task_queue.push(task);

        read_count++;
        total_length += task.read.size();

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

    generateMinhashSignature(lcp_cores);

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

    if (argc < 3) {
        std::cerr << "Wrong format: " << argv[0] << " [genome-input-1] [genome-input-2] -t [lcp-level] -t [threads] --genome-1-len [number] -genome-2-len [number]" << std::endl;
        return -1;  
    }

    argp::parser parser;
    parser.parse(argc, argv);

    std::string arg_lcp_level, arg_threads;
    size_t thread_number = THREAD_NUMBER;
    std::string genome_length_1, genome_length_2;
    size_t genome_1_length, genome_2_length;

    parser({"-l"}) >> arg_lcp_level;
    parser({"-t"}) >> arg_threads;
    parser({"--genome-1-len"}) >> genome_length_1;
    parser({"--genome-2-len"}) >> genome_length_2;

    genome_1_length = std::stoull(genome_length_1);
    genome_2_length = std::stoull(genome_length_2);

    std::cout << "Size of the genome 1 is set to " << genome_1_length << std::endl;
    std::cout << "Size of the genome 2 is set to " << genome_2_length << std::endl;

    if ( arg_threads != "" ) {
        try {
            thread_number =  static_cast<size_t>(std::stoull(arg_threads));
            std::cout << "Thread number is set to " << thread_number << " per genome" << std::endl;
        } catch (...) {
            std::cerr << "Unable to convert threads parameter to size_t" << std::endl;
        }
    } else {
        std::cout << "Default thread number is set to " << thread_number << " per genome" << std::endl;
        std::cout << "Warning: 2 more thread is needed for reading 2 genomes and assiging tasks to others." << std::endl;
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

    int lcp_level;
    try {
        lcp_level = std::stoi(arg_lcp_level);
        std::cout << "LCP Level is set to " << lcp_level << std::endl;
    } catch (...) {
        std::cerr << "Unable to convert LCP Level parameter to int" << std::endl;
        exit(-1);
    }

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    std::vector<uint> lcp_cores_1, lcp_cores_2;
    lcp_cores_1.reserve(1000000000);
    lcp_cores_2.reserve(1000000000);

    size_t read_count_1 = 0, read_count_2 = 0;
    size_t total_length_1 = 0, total_length_2 = 0;

    // create thread to process first genome in parallel
    std::thread thread1(processGenome, argv[1], std::ref(infile1), lcp_level, std::ref(lcp_cores_1), std::ref(read_count_1), std::ref(total_length_1), thread_number);
    
    // process second genome in main thread, no need for running in seperate thread
    processGenome(argv[2], infile2, lcp_level, lcp_cores_2, read_count_2, total_length_2, thread_number);
    
    // wait for thread to finish
    thread1.join();

    double depth_1 = static_cast<double>(total_length_1) / genome_1_length;
    double depth_2 = static_cast<double>(total_length_2) / genome_2_length;
    
    std::cout << "Processing is done for " << argv[1] << std::endl;
    std::cout << "Total length of reads calculated as " << total_length_1 << std::endl;
    std::cout << "Depth is calculated as " << depth_1 << std::endl;
    std::cout << "Total number of processed reads count: " << read_count_1 << std::endl;
    std::cout << "Total number of cores: " << lcp_cores_1.size() << std::endl;

    std::cout << "Processing is done for " << argv[2] << std::endl;
    std::cout << "Total length of reads calculated as " << total_length_2 << std::endl;
    std::cout << "Depth is calculated as " << depth_2 << std::endl;
    std::cout << "Total number of processed reads count: " << read_count_2 << std::endl;
    std::cout << "Total number of cores: " << lcp_cores_2.size() << std::endl; 

    size_t intersection_size = 0, union_size = 0, same_count;

    calculateIntersectionAndUnionSizes(lcp_cores_1, lcp_cores_2, intersection_size, union_size);

    double jaccard_similarity = calculateJaccardSimilarity(intersection_size, union_size);
    double dice_similarity = calculateDiceSimilarity(intersection_size, lcp_cores_1.size(), lcp_cores_2.size());
    double distance_similarity = calculateDistanceSimilarity(lcp_cores_1, lcp_cores_2, depth_1, depth_2, same_count);

    std::vector<int> uniqueValues;
    std::vector<int> counts;

    std::cout << "Intersection Size: " << intersection_size << std::endl;
    std::cout << "Union Size: " << union_size << std::endl;
    std::cout << "Same Core Count: " << same_count << std::endl;
    std::cout << std::endl;
    std::cout << "Jaccard Similarity: " << jaccard_similarity << std::endl;
    std::cout << "Dice Similarity: " << dice_similarity << std::endl;
    std::cout << "Distance Based Similarity: " << distance_similarity << std::endl;
    std::cout << std::endl;

    return 0;
};
