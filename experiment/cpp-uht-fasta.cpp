#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <chrono>
#include "../string.cpp"

#define DISTANCE_LENGTH     10000
#define LCP_LEVEL           4
#define KMER_SIZE           15


struct uht_kmer {
    int label;
    int pos;
};


double mean(int (&distances)[DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = 0;
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        m += i * distances[i];
        count += distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        m += larger_distances[i];
    }
    count += larger_distances.size();
    return m / count;
};


double std_deviation(int (&distances)[DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = mean(distances, larger_distances);
    double count = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        count += distances[i];
    }
    count += larger_distances.size();
    double variance = 0;
    for ( int i = 0; i < DISTANCE_LENGTH; i++ ) {
        variance += ( m - i ) * ( m - i ) * distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        variance += ( m - larger_distances[i] ) * ( m - larger_distances[i] );
    }
    return sqrt(variance / count);
};


double mean_shifted(int (&distances)[2*DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = 0;
    double count = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        m += (i-DISTANCE_LENGTH) * distances[i];
        count += distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        m += larger_distances[i];
    }
    count += larger_distances.size();
    return m / count;
};


double std_deviation_shifted(int (&distances)[2*DISTANCE_LENGTH], std::vector<int> larger_distances = {}) {
    double m = mean_shifted(distances, larger_distances);
    double count = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        count += distances[i];
    }
    count += larger_distances.size();
    double variance = 0;
    for ( int i = 0; i < 2*DISTANCE_LENGTH; i++ ) {
        variance += ( m - (i-DISTANCE_LENGTH) ) * ( m - (i-DISTANCE_LENGTH) ) * distances[i];
    }
    for ( int i = 0; i < larger_distances.size(); i++ ) {
        variance += ( m - larger_distances[i] ) * ( m - larger_distances[i] );
    }
    return sqrt(variance / count);
};


void print2file(int (&all_distances)[2*DISTANCE_LENGTH], int (&all_distances_pos)[DISTANCE_LENGTH], std::vector<int> &all_larger_distances_vec, std::vector<int> &all_larger_distances_pos_vec, std::ofstream &outfile ) { 
    
    outfile << "Distances ( core_curr->pos - core_prev->end ) " << std::endl;
    for ( int j = 0; j < 2*DISTANCE_LENGTH; j++ ) {
        for ( int k = 0; k < all_distances[j]; k++ ) {
            outfile << j - DISTANCE_LENGTH << ',';
        }
    }
    for ( int j = 0; j < all_larger_distances_vec.size(); j++ ) {
        outfile << all_larger_distances_vec[j] << ',';
    }
    outfile << std::endl;

    outfile << "Distances btw pos ( core_curr->pos - core_prev->pos ) " << std::endl;
    for ( int j = 0; j < DISTANCE_LENGTH; j++ ) {
        for ( int k = 0; k < all_distances_pos[j]; k++ ) {
            outfile << j << ',';
        }
    }
    for ( int j = 0; j < all_larger_distances_pos_vec.size(); j++ ) {
        outfile << all_larger_distances_pos_vec[j] << ',';
    }

    outfile << std::endl << std::endl;
};


void summary( int &overlapping_counts, int (&all_distances)[2*DISTANCE_LENGTH], int (&all_distances_pos)[DISTANCE_LENGTH], std::vector<int> &all_larger_distances_vec, std::vector<int> &all_larger_distances_pos_vec, std::chrono::milliseconds &all_durations, int &all_core_count) {

    std::cout << "Overlapping core counts:                  " << overlapping_counts << std::endl;
    std::cout << "Std of distances btw cores (w'out):       " << std_deviation_shifted(all_distances) << std::endl;
    std::cout << "Mean of distances btw cores (w'out):      " << mean_shifted(all_distances) << std::endl;
    std::cout << "Std of distances btw cores (with):        " << std_deviation_shifted(all_distances, all_larger_distances_veC) << std::endl;
    std::cout << "Mean of distances btw cores (with):       " << mean_shifted(all_distances, all_larger_distances_vec) << std::endl;

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "Std of distances btw starts (w'out):      " << std_deviation(all_distances_pos) << std::endl;
    std::cout << "Mean of distances btw starts (w'out):     " << mean(all_distances_pos) << std::endl;
    std::cout << "Std of distances btw starts (with):       " << std_deviation(all_distances_pos, all_larger_distances_pos_vec) << std::endl;
    std::cout << "Mean of distances btw starts (with):      " << mean(all_distances_pos, all_larger_distances_pos_vec) << std::endl;

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "Std of lengths:                           " << KMER_SIZE << std::endl;
    std::cout << "Mean of lengths:                          " << 0 << std::endl;

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "Level execution time:                     " << ( (double) all_durations.count() ) / 1000 << " sec" << std::endl;
    std::cout << "Total number of cores:                    " << all_core_count << std::endl;

    std::cout << "------------------------------------------" << std::endl;

    std::cout << "dist # not in [-10k,10k):                 " << all_larger_distances_vec.size() << std::endl;
    std::cout << "dist btw pos # not in [0,10k):            " << all_larger_distances_pos_vec.size() << std::endl; 
    std::cout << "length # [0,10k):                         " << all_larger_lengths_vec.size() << std::endl;
    std::cout << std::endl;
};


void analyze( int &overlapping_counts, int (&all_distances)[2*DISTANCE_LENGTH], int (&all_distances_pos)[DISTANCE_LENGTH], std::vector<int> &all_larger_distances_vec, std::vector<int> &all_larger_distances_pos_vec, std::vector<uht_kmer> elements) {
    
    for ( std::vector<uht_kmer>::iterator it = elements.begin() + 1; it < elements.end(); it++ ) {

        if ( (*it)->pos < (*(it-1))->pos + KMER_SIZE ) {
            overlapping_counts += 1;
        }
        if ( 0 <= (*it)->pos - (*(it-1))->pos + KMER_SIZE + DISTANCE_LENGTH && (*it)->pos - (*(it-1))->pos + KMER_SIZE < 2*DISTANCE_LENGTH ) {
            all_distances[(*it)->pos - (*(it-1))->pos + KMER_SIZE + DISTANCE_LENGTH]++;
        } else {
            all_larger_distances_vec.push_back((*it)->pos - (*(it-1))->pos + KMER_SIZE);
        }

        if ( 0 <= (*it)->pos - (*(it-1))->pos && (*it)->pos - (*(it-1))->pos < DISTANCE_LENGTH ) {
            all_distances_pos[(*it)->pos - (*(it-1))->pos]++;
        } else {
            all_larger_distances_pos_vec.push_back( (*it)->pos - (*(it-1))->pos );
        }
    }
};


int main(int argc, char **argv) {

    if (argc < 2) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] " << std::endl;
        return -1;  
    }

    std::ifstream input(argv[1]);
    if (!input.good()) {
        std::cerr << "Error opening: " << argv[1] << " . You have failed." << std::endl;
        return -1;
    }

    std::ifstream input2(argv[2]);
    if (!input2.good()) {
        std::cerr << "Error opening: " << argv[2] << " . You have failed." << std::endl;
        return -1;
    }

    // initializing coefficients of the alphabet
    lcp::init_coefficients();

    // variables
    std::string uht_file, line;

    std::fstream genome;
    genome.open(argv[1], std::ios::in);
    uht_file.open(argv[2], std::ios::in);

    // assuming dict_bit_size <= 2 and KMER_SIZE <= 16
    bool uht_set[2 ** (dict_bit_size * KMER_SIZE)] = {false};
    int uht_value;
    if ( uht_fil ) {
        while ( getline(uht_file, uht_line) ) {
            uht_value = 0;
            for ( std::string::iterator it = uht_line.begin(); it < uht_line.end(); it++ ) {
                uht_value *= dict_bit_size;
                uht_value |= coefficients[(*it)];
            }
            uht_set[uht_value] = true;
        }
    } else {
        exit(-1);
    }

    int chrom_index = 0;
    int all_overlapping_counts = 0;
    int all_core_count = 0;
    int all_distances[2*DISTANCE_LENGTH] = 0;  // in order to store overlapping cores as well (as distance will be negative)
    int all_distances_pos[DISTANCE_LENGTH] = 0;
    std::chrono::milliseconds all_durations;
    std::vector<int> all_larger_distances_vec;
    std::vector<int> all_larger_distances_pos_vec;,
    std::vector<uht_kmer> elements;

	// read file
    if ( genome.is_open() ) {  

        std::string sequence, id;
        sequence.reserve(250000000);
        
        std::cout << "Program begins" << std::endl;

        while (getline(genome, line)) {

            if (line[0] == '>') {

                chrom_index++;

                // process previous chromosome before moving into new one
                if (sequence.size() != 0) {
                    
                    elements.clear();
                    int curr_value = 0, index = 0, mask = 2**30 - 1;

                    std::string::iterator it = sequence.begin();
                    for ( index < KMER_SIZE - 1 && it < sequence.end(); index++, it++ ) {
                        curr_value *= dict_bit_size;
                        curr_value |= coefficients[(*it)];
                    }

                    int curr_kmer_size = KMER_SIZE;

                    for ( ;  it < sequence.end(); it++, index++ ) {
                        if ( (*it) != 'N' ) {
                            curr_value *= dict_bit_size;
                            
                            if ( curr_kmer_size == KMER_SIZE ) {
                                curr_value &= mask;
                            } else {
                                curr_kmer_size++;
                            }
                            
                            curr_value |= coefficients[(*it)];

                            if ( curr_kmer_size == KMER_SIZE && uht_set[curr_value] ) {
                                elements.push_back(uht_kmer(curr_value, index));
                            }
                        } else {
                            curr_value = 0;
                            curr_kmer_size = 0;
                        }
                    }

                    analyze( all_overlapping_counts, all_distances, all_distances_pos, all_larger_distances_vec, all_larger_distances_pos_vec, elements);

                    elements.clear();
                    sequence.clear();
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

        summary( all_overlapping_counts, all_distances, all_distances_pos, all_larger_distances_vec, all_larger_distances_pos_vec, all_durations, all_core_count);

        genome.close();
    }

    std::ofstream outfile ( "detailed_summary.uht.fasta.txt" );
    
    print2file( all_distances, all_distances_pos, all_larger_distances_vec, all_larger_distances_pos_vec, outfile );

    outfile.close();

    return 0;
};
