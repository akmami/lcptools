#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <cstring>
#include <functional>
#include "../utils/GzFile.hpp"
#include "../string.cpp"

#define IDSIZE              128
#define MIN_PATH_LENGTH     3
#define MAX_TRY_COLLECT     10
#define LCP_LEVEL           5


// Hash function type
using hash = std::function<int(int)>;

// trie_leaf to hold read IDs
struct trie_leaf {
    std::vector<size_t> ids;
};

// trie_node structure
struct trie_node {
    std::unordered_map<uint, struct trie_node*> children;
    trie_leaf* leaf = NULL;
    size_t num_leaves = 0;

    ~trie_node() {
        for (std::unordered_map<uint, struct trie_node*>::iterator it = children.begin(); it != children.end(); ++it) {
            delete it->second;
        }
        if(leaf)
            delete leaf;
    }
};

// insert into Trie
void insert(struct trie_node* root, std::vector<uint>& read, size_t id) {
    
    struct trie_node* node = root;

    node->num_leaves++;
    
    for (std::vector<uint>::iterator it = read.begin(); it != read.end(); it++) {

        if (it != read.begin() && *(it-1) == *(it) ) {
            continue;
        }

        if (node->children.find(*it) == node->children.end()) {
            node->children[*it] = new struct trie_node();
        }
        node = node->children[*it];
        node->num_leaves++;
    }

    if (!node->leaf) {
        node->leaf = new trie_leaf();
    }
    node->leaf->ids.push_back(id);
};

// remove from Trie
void remove(struct trie_node* root, std::vector<uint>& read, size_t id) {

    struct trie_node* node = root;

    for (std::vector<uint>::iterator it = read.begin(); it != read.end(); it++) {
        
        if (node->children.find(*it) == node->children.end()) {
            continue;
        }
        
        node->num_leaves--;

        if ( node->children[*it]->num_leaves == 1 ) {
            delete node->children[*it];
            node->children.erase(*it);

            return;
        }
        node = node->children[*it];
    }

    if (node->leaf) {
        // Remove the ID from the leaf. As num_leaves != 1, only removal from leaf needs to be done.
        std::vector<size_t>::iterator it_pos = std::find(node->leaf->ids.begin(), node->leaf->ids.end(), id);
        
        if (it_pos != node->leaf->ids.end()) {
            node->leaf->ids.erase(it_pos);
        }
    }
}

void collect(struct trie_node* node, std::vector<size_t>& ids, size_t depth, size_t& id) {
    if (!node) return;

    if (depth == 0 && node->leaf) {
        for (std::vector<size_t>::iterator it = node->leaf->ids.begin(); it != node->leaf->ids.end(); it++) {
            if ( (*it) != id) {
                ids.push_back(*it);
            }
        }
        return;
    }

    if (depth == 0) {
        return;
    }
    
    for (std::unordered_map<uint, struct trie_node *>::iterator it = node->children.begin(); it != node->children.end(); it++) {
        collect((*it).second, ids, depth - 1, id);
    }
};

uint find_best_overlap(struct trie_node* root, std::vector<uint>& read, size_t& id, struct trie_node* &lastDivergentNode) {
    
    if (!root) {
        return 0;
    }

    struct trie_node* current = root;
    uint length = 0;

    for (std::vector<uint>::iterator it = read.begin(); it != read.end(); it++) {

        if (it != read.begin() && *(it-1) == *(it) ) {
            continue;
        }

        std::unordered_map<uint, struct trie_node *>::iterator found = current->children.find(*it);
        
        if (found != current->children.end()) {
            // check if there's a divergence
            if (current->num_leaves > 1) {
                lastDivergentNode = current;
                length++;

                // move to the child
                current = found->second;
                
                continue;
            }
        }

        break;
    }

    if (lastDivergentNode == root || length < MIN_PATH_LENGTH) {
        return 0;
    }

    return length;
};


int main(int argc, char** argv) {
    
    // validate arguments
    if (argc != 3) {
        std::cerr << "Wrong format: " << argv[0] << " [infile] [outfile]" << std::endl;
        return -1;  
    }

    // validate input file
    GzFile infile( argv[1], "rb");
    if (!infile) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    // open output file
    std::ofstream outfile( argv[2] );
    if ( !outfile ) {
        std::cerr << "Error creating outfile " << argv[2] << std::endl;
        return -1;
    }

    const size_t hashing_function_size = 41;

    // needs bitwise operation for significant order change and modulo with prime number
    hash array_of_functions[hashing_function_size] = {
        [](int x) -> int { return x; },
        [](int x) -> int { return (x ^ 0x5b5) ^ (x >> 7); },
        [](int x) -> int { return (x * 0x1a7) ^ (x << 9); },
        [](int x) -> int { return (x ^ 0x2b79); },
        [](int x) -> int { return (x * 0x173) ^ (x << 5) ^ (x >> 2); },
        [](int x) -> int { return (x ^ (x << 9)) * 0x1b3c9; },
        [](int x) -> int { return (x * 0x1651) ^ (x + 0xfd7); },
        [](int x) -> int { return (x ^ 0xc76) ^ (x * 0x7b1); },
        [](int x) -> int { return (x + 0x55cb) ^ (x << 9); },
        [](int x) -> int { return (x * 0x75) ^ (x * 0x1a2d); },
        [](int x) -> int { return (x + 0x2b3cd) ^ (x >> 12); },
        [](int x) -> int { return (x * 0x12377) + (x >> 9); },
        [](int x) -> int { return (x ^ 0x3cef3) - (x << 11); },
        [](int x) -> int { return (x * 0x1e1a9) + (x * 0x49f); },
        [](int x) -> int { return (x ^ (x * 0x41d)) + 0x6073; },
        [](int x) -> int { return (x * 0x27dd) - (x >> 13); },
        [](int x) -> int { return (x + 0x4a0d) ^ (x * 0x12f9); },
        [](int x) -> int { return (x ^ 0x5d65) * (x + 0x17d3); },
        [](int x) -> int { return (x * 0x4578) ^ (x >> 14); },
        [](int x) -> int { return (x ^ 0x17) ^ (x * 0x0456); },
        [](int x) -> int { return (x * 0x0fcf) + (x ^ 0x101); },
        [](int x) -> int { return ((x ^ 0x3d9d) + (x << 7)) % 1000000007; },
        [](int x) -> int { return ((x * 0x1a) - (x >> 5)) % 1000000009; },
        [](int x) -> int { return ((x + 0x6e2f) ^ (x << 13)) % 1000000021; },
        [](int x) -> int { return ((x * 0x2f3) - (x << 11)) % 1000000033; },
        [](int x) -> int { return ((x ^ 0x4f74d2) + (x * 31)) % 1000000087; },
        [](int x) -> int { return ((x * 0x3381) ^ (x + 0xfca6f) ) % 1000000093; },
        [](int x) -> int { return ((x + 0x12) * (x << 5)) % 1000000097; },
        [](int x) -> int { return ((x ^ 0x83f) - (x >> 7)) % 1000000103; },
        [](int x) -> int { return ((x * 0x3) ^ (x * 53)) % 1000000123; },
        [](int x) -> int { return ((x + 0x7e) - (x << 9)) % 1000000181; },
        [](int x) -> int { return ((x * 0x9) + (x >> 12)) % 1000000207; },
        [](int x) -> int { return ((x ^ 0x5f9) * (x << 15)) % 1000000223; },
        [](int x) -> int { return ((x + 0x124) ^ (x * 0x41ce6d)) % 1000000241; },
        [](int x) -> int { return ((x * 0x27) + (x >> 4)) % 1000000271; },
        [](int x) -> int { return ((x ^ 0xabde) - (x * 79)) % 1000000321; },
        [](int x) -> int { return ((x * 0x3f2) + (x << 6)) % 1000000347; },
        [](int x) -> int { return ((x ^ 0xdea) ^ (x >> 4)) % 1000000363; },
        [](int x) -> int { return ((x + 0xd) * (x << 17)) % 1000000393; },
        [](int x) -> int { return ((x * 0xfeece) ^ (x + 0x12568)) % 1000000403; },
        [](int x) -> int { return ((x ^ 0xfacd) + (x >> 3)) % 1000000423; }
    };
    
    // init trie tree
    struct trie_node* trie_roots[hashing_function_size];
    size_t scores[hashing_function_size] = { 0 };
    
    for (size_t i = 0; i < hashing_function_size; ++i) {
        trie_roots[i] = new struct trie_node();
    }
    
    std::vector<std::string> strings;
    strings.reserve(100000000);
    std::vector<std::array<std::vector<uint>, hashing_function_size>> reads;
    reads.reserve(100000000);
    std::map<std::string, size_t> mapped;
    size_t index = 0;

    // init lcp
    lcp::init_coefficients();
    
    char buffer[BUFFERSIZE];

    // Write Header
    outfile << "H\tVN:Z:1.0\n";

    while (true) {
        
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }

        // Get id
        buffer[strlen(buffer)-1] = '\0';
        std::string id(buffer+1);

        strings.push_back(id);
        mapped[id] = index;

        // Write id to gfa
        outfile << "S\t" << id << "\t.\n"; // put . instead of sequence string

        // Get read
        infile.gets(buffer, BUFFERSIZE);
        buffer[strlen(buffer)-1] = '\0';
        std::string read(buffer);

        // Process read
        lcp::string* lcp = new lcp::string(read);

        for ( int i = 0; i < LCP_LEVEL; i++ ) {
            lcp->deepen();
        }

        std::array<std::vector<uint>, hashing_function_size> cores;
        
        // Get hash the values
        for (uint i = 0; i < hashing_function_size; ++i) {
            for( std::vector<lcp::core*>::iterator it = lcp->cores.begin(); it != lcp->cores.end(); it++ ) {
                cores[i].push_back(array_of_functions[i]( (*it)->label ));
            }
            // Sort the hashed values
            std::sort(cores[i].begin(), cores[i].end());
            
            // Insert into Trie
            insert(trie_roots[i], cores[i], index);
        }

        reads.push_back(cores);

        delete lcp;
        index++;
        
        // Read quality scores
        infile.gets(buffer, BUFFERSIZE);
        infile.gets(buffer, BUFFERSIZE);
    }

    bool processed[index] = { false };
    index = 0;

    for (std::vector<std::array<std::vector<uint>, hashing_function_size>>::iterator it = reads.begin(); it != reads.end(); it++, index++ ) {
        
        if ( ! processed[index] ) {

            size_t curr_index = index;
            std::vector<std::array<std::vector<uint>, hashing_function_size>>::iterator curr_it = it;

            while(true) {

                std::vector<size_t> result;
                size_t max_length = 0, length, max_index;
                struct trie_node* last_divergent_node = NULL;
                struct trie_node* last_best_divergent_node = NULL;
            
                for (size_t i = 0; i < hashing_function_size; ++i) {

                    length = find_best_overlap(trie_roots[i], (*curr_it)[i], curr_index, last_divergent_node);
                    
                    if (length > max_length) {
                        last_best_divergent_node = last_divergent_node;
                        max_length = length;
                        max_index = i;
                    }
                }

                if (last_best_divergent_node != NULL) {

                    for( size_t i = 0; result.size() == 0 && i < MAX_TRY_COLLECT; i++) {
                        collect(last_best_divergent_node, result, i, curr_index);
                    }

                    std::vector<size_t>::iterator result_it;
                    
                    // // Print best possible results for id
                    // std::cout << strings[curr_index] << ": ";
                    // for (result_it = result.begin(); result_it != result.end(); result_it++) {
                    //     std::cout << strings[*result_it] << " ";
                    // }
                    // std::cout << std::endl;

                    // Remove read from trie tree
                    for (uint i = 0; i < hashing_function_size; ++i) {
                        remove(trie_roots[i], (*curr_it)[i], curr_index);
                    }

                    processed[curr_index] = true;

                    for ( result_it = result.begin(); result_it != result.end() && processed[*result_it]; result_it++ ) {}

                    if ( result_it != result.end() ) {
                        if ( (*result_it) < index ) {
                            break;
                        }

                        scores[max_index]++;

                        outfile << "L\t" << strings[curr_index] << "\t+\t" << strings[*result_it] << "\t-\t" << 0 << "M\n";

                        std::cout << strings[curr_index] << " " << strings[*result_it] << std::endl;

                        curr_index = *result_it;

                        continue;
                    }
                }

                break;
            }

            // Insert into Trie
            for (uint i = 0; i < hashing_function_size; ++i) {
                insert(trie_roots[i], (*it)[i], index);
            }
            processed[curr_index] = false;
        }
    }

    for (uint i = 0; i < hashing_function_size; ++i) {
        std::cout << i << ": " << scores[i] << " ";
    }
    std::cout << std::endl;

    // clean up
    for (uint i = 0; i < hashing_function_size; ++i) {
        delete trie_roots[i];
    }

    return 0;
}
