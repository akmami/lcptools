/**
 * @file    InvertedIndex.hpp
 * @brief   Inverted Index for Genomic Read Retrieval
 *
 * Implements an inverted index data structure optimized for genomic read retrieval.
 * Each read is processed to extract significant features (e.g., lcp cores), which are
 * then represented as small integers. These integers are inserted into the inverted
 * index, allowing efficient querying for reads based on the occurrence of specific
 * features. The structure supports operations such as insertion of reads, querying
 * for similar reads based on feature overlap, and summarizing the index contents.
 *
 * Usage scenarios include genomic data analysis where quick retrieval of reads with
 * similar sequences or patterns is necessary, facilitating tasks such as read mapping,
 * assembly, similarity searches, and variant analysis.
 */


#ifndef INVERTEDINDEX_HPP
#define INVERTEDINDEX_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include "../utils/GzFile.hpp"
#include "../string.cpp"

#define ANALYZE_ARRAY_SIZE  10000


typedef size_t doc_id_type;
typedef uint core_label_type;
typedef std::unordered_map<core_label_type, std::vector<doc_id_type>> inverted_index_type;
typedef std::vector<std::pair<std::vector<doc_id_type>::const_iterator, std::vector<doc_id_type>::const_iterator>> iterators_type;


class InvertedIndex {
private:
    inverted_index_type index;

public:
    /**
     * @brief Inserts a document (read) ID into the inverted index based on labels from lcp::string cores.
     *
     * This method extends the functionality of the InvertedIndex class to work directly with 
     * lcp::string objects, which are presumably representations of genomic sequences or reads.
     * Each core within the lcp::string object is associated with a label, which is used as the
     * key in the inverted index. The document (read) ID is added to the list of IDs associated 
     * with each label encountered in the cores of the lcp::string.
     *
     * @param read_id The unique identifier of the read being inserted. This ID is associated 
     *                with each label found in the cores of the provided lcp::string.
     * @param lcp A pointer to an lcp::string object, which contains cores with labels that 
     *            represent segments of the genomic sequence.
     */
    void insert(const doc_id_type &read_id, lcp::string *lcp) {
        for (std::vector<lcp::core *>::iterator it = lcp->cores.begin(); it != lcp->cores.end(); it++) {
            index[(*it)->label].push_back(read_id);
        }
    }


    /**
     * @brief Retrieves the document ID with the maximum count of matching features from an lcp::string.
     *
     * This method searches the inverted index for the document (read) that has the maximum number
     * of matching features (cores) as those specified in the provided lcp::string object, excluding
     * a specified document ID. It is particularly useful for finding the most similar genomic read
     * within a collection, based on a set of features or patterns.
     *
     * @param id The document (read) ID to exclude from the search. This is typically the ID of the
     *           query read itself, to prevent it from being identified as its own best match.
     * @param lcp A pointer to an lcp::string object containing the cores (features) to match against
     *            the documents in the index. Each core's label is used as a key to find matching
     *            document IDs in the index.
     * @return A std::pair where the first element is the document ID with the highest number of matching
     *         features (excluding the specified ID), and the second element is the count of these matches.
     *        
     */
    std::pair<doc_id_type, size_t> retrieveMaxDocIdAndCount(const doc_id_type &id, lcp::string *lcp) {
        
        // This structure will hold the current position in the list of document IDs for each term
        iterators_type iterators;

        iterators.reserve(lcp->cores.size());
        
        std::cout << "Assigning iterators" << std::endl;

        for (std::vector<lcp::core *>::iterator core = lcp->cores.begin(); core != lcp->cores.end(); core++) {
            
            inverted_index_type::iterator it = this->index.find((*core)->label);
            
            if (it != this->index.end()) {
                // Add iterators to the beginning and end of the document list for this term
                iterators.push_back(std::make_pair(it->second.begin(), it->second.end()));
            }
        }

        std::cout << "Iterators assigned" << std::endl;

        size_t maxCount = 0;
        doc_id_type maxDocId = std::numeric_limits<doc_id_type>::max();
        
        bool allIteratorsAtEnd = false;

        std::cout << "Iterating in inverted index" << std::endl;

        while ( true ) {

            // for (iterators_type::iterator it = iterators.begin(); it != iterators.end(); it++ ) {
            //     if ( it->first != it->second ) {
            //         std::cout << *(it->first) << "\t";
            //     } else {
            //         std::cout << "end\t";   
            //     }
            // }

            // std::cout << std::endl;

            
            // Find the minimum and maximum document ID among the current positions of the iterators
            doc_id_type minDocId = std::numeric_limits<doc_id_type>::max();
            size_t minDocCount = 0;
            
            allIteratorsAtEnd = true;

            for (iterators_type::iterator it = iterators.begin(); it != iterators.end(); it++ ) {
                
                if (it->first != it->second) {
                    
                    allIteratorsAtEnd = false;

                    if ( *(it->first) == minDocId ) {
                        minDocCount++;
                    } else if ( *(it->first) < minDocId ) {
                        minDocCount = 0;
                        minDocId = *(it->first);
                    }
                }
            }

            if ( allIteratorsAtEnd ) {
                std::cout << "All iterators ended." << std::endl;
                break; // All iterators have reached the end
            }

            if ( minDocCount > maxCount && minDocId != id ) {
                maxCount = minDocCount;
                maxDocId = minDocId;
            }

            // Move all iterators pointing to the minDocId to the next document ID
            for ( iterators_type::iterator it = iterators.begin(); it != iterators.end(); it++ ) {
                if ( it->first != it->second && *(it->first) == minDocId ) {
                    (it->first)++;
                }
            }
        }

        std::cout << "Max count: " << maxCount << ", Max ID: " << maxDocId << std::endl;

        return { maxDocId, maxCount };
    }


    /**
     * @brief Prints the contents of the inverted index to a specified output file.
     * 
     * Iterates through the index and prints each term along with the list of document
     * IDs (read IDs) associated with that term. This is useful for examining the contents
     * of the index or for debugging purposes.
     * 
     * @param outfile A GzFile object for output, supporting gzip-compressed files.
     */
    void print(GzFile &outfile) {
        
        outfile.printf("Inverted Index Summary:\n");
        
        for (inverted_index_type::const_iterator it = index.begin(); it != index.end(); ++it) {
            core_label_type term = it->first;
            const std::vector<doc_id_type>& docs = it->second;
            
            outfile.printf("Term %d: ", term);
            
            for (std::vector<doc_id_type>::const_iterator docIt = docs.begin(); docIt != docs.end(); ++docIt) {
                
                outfile.printf("%d", *docIt);
                
                if (docIt != docs.end() - 1) {
                    outfile.printf(", ");
                }
            }
            
            outfile.printf("\n");
        }
        
        outfile.printf("\n");
    }


    /**
     * @brief Summarizes the contents of the inverted index on the standard output.
     * 
     * Similar to print, but instead writes the summary of the index to the standard output.
     * Each term and its associated read IDs are printed, providing a quick overview of the
     * index contents.
     */
    void summary() {
        
        std::cout << "Inverted Index Summary: " << std::endl;
        
        for (inverted_index_type::const_iterator it = index.begin(); it != index.end(); ++it) {
            core_label_type term = it->first;
            const std::vector<doc_id_type>& docs = it->second;
            
            std::cout << "Term " << term << ": ";
            
            for (std::vector<doc_id_type>::const_iterator docIt = docs.begin(); docIt != docs.end(); ++docIt) {
                
                std::cout << *docIt;
                
                if (docIt != docs.end() - 1) {
                    std::cout << ", ";
                }
            }
            
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
    }


    /**
     * @brief Analyzes and outputs statistics about the distribution of reads across LCP core values in the inverted index.
     *
     * This function compiles statistics on the distribution of reads (document IDs) associated with specific LCP core values
     * within the inverted index at a given LCP analysis level. It calculates the frequency of LCP core values associated with
     * varying numbers of reads, identifying core values linked to large numbers of reads. The results, including the
     * distribution of read counts for LCP core values and the count of core values associated with a higher than expected
     * number of reads, are output to a specified file. This analysis aids in understanding the diversity and repetition of
     * genomic sequences at the specified level of LCP analysis, providing insights into the genomic data's complexity.
     *
     * @param outfile Reference to an ofstream object for writing the analysis results. The output includes detailed
     *                statistics on the distribution of reads among LCP core values, highlighting both common and rare
     *                genomic patterns as represented by the inverted index.
     * @param lcp_level The LCP level at which the analysis is performed, indicating the granularity or depth of
     *                  sequence analysis for identifying LCP core values. The LCP level directly impacts the interpretation
     *                  of genomic sequence complexity and repetitiveness.
     */
    void analyze(std::ofstream &outfile, int lcp_level) {

        size_t counts[ANALYZE_ARRAY_SIZE] = { 0 };
        std::vector<size_t> larger_values;

        for (inverted_index_type::const_iterator it = index.begin(); it != index.end(); ++it) {
            // core_label_type term = it->first;
            // const std::vector<doc_id_type>& docs = it->second;

            if ( it->second.size() < ANALYZE_ARRAY_SIZE ) {
                counts[it->second.size()]++;
            } else {
                larger_values.push_back( it->second.size() );
            }
        }

        outfile << "(" << lcp_level << ", [" << counts[0];

        for ( size_t i = 1; i < ANALYZE_ARRAY_SIZE; i++ ) {
            outfile << "," << counts[i];
        }
        outfile << "]),     #  - " << lcp_level << std::endl;
        outfile << "Number of terms that has size larger than " << ANALYZE_ARRAY_SIZE << " is " << larger_values.size() << std::endl;
        outfile << "Number of terms is " << this->index.size() << std::endl;
    }
};


#endif