#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cstring>
#include "GzFile.hpp"
#include "../string.cpp"

#define IDSIZE      128


// Hash function for integers
int hash1(int number) {
    return number;
};

// trie_leaf to hold read IDs
struct trie_leaf {
    std::set<std::array<char, IDSIZE>> ids;
};

// trie_node structure
struct trie_node {
    std::unordered_map<int, trie_node*> children;
    trie_leaf* leaf = nullptr;          // Pointer to a leaf, null if this node is not a leaf

    ~trie_node() {
        for (std::unordered_map<int, trie_node*>::iterator it = children.begin(); it != children.end(); ++it) {
            delete it->second;
        }
        if(leaf)
            delete leaf;
    }
};

// Insert into Trie
void insert(trie_node* root, const std::vector<int>& hashedRead, std::array<char, IDSIZE> id) {
    
    trie_node* node = root;
    
    for (int num : hashedRead) {
        if (node->children.find(num) == node->children.end()) {
            node->children[num] = new trie_node();
        }
        node = node->children[num];
    }

    if (!node->leaf) {
        node->leaf = new trie_leaf();
    }
    node->leaf->ids.insert(id);
};

std::pair<int, std::set<std::array<char, IDSIZE>>> findMostSimilarRead(trie_node* root, const std::vector<int>& read, std::array<char, IDSIZE> id) {
    
    if (!root) {
        return {0, {}};
    }

    trie_node* current = root;
    trie_node* lastDivergentNode = nullptr;
    int length = 0;

    for (int num : read) {
        auto it = current->children.find(num);
        if (it != current->children.end()) {
            // Move to the child
            current = it->second;
            length++;

            // Check if there's a divergence (more than one child or a leaf with different IDs)
            if (current->children.size() > 1 || (current->leaf && current->leaf->ids.size() > 1 && current->leaf->ids.count(id) == 0)) {
                lastDivergentNode = current;
            }
        } else {
            break; // No further path
        }
    }

    // Backtrack to the last divergent node if the current node is a leaf with only the id
    if (current->leaf && current->leaf->ids.size() == 1 && current->leaf->ids.count(id) == 1 && lastDivergentNode) {
        current = lastDivergentNode;
    }

    if (current && current->leaf) {
        // Return the IDs from the leaf excluding the id
        std::set<std::array<char, IDSIZE>> resultIds = current->leaf->ids;
        resultIds.erase(id);
        return {length, resultIds};
    }

    return {length, {}}; // Return the length and empty set if no suitable leaf found
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
    GzFile outfile( argv[2], "wb" );
    if ( !outfile ) {
        std::cerr << "Error creating outfile " << argv[2] << std::endl;
        return -1;
    }

    char buffer[BUFFERSIZE];
    trie_node* root = new trie_node();
    int bytesRead;

    while (true) {
        
        if ( infile.gets(buffer, BUFFERSIZE) == Z_NULL) {
            // End of file or an error
            if ( ! infile.eof() ) {
                std::cerr << "Error reading file." << std::endl;
            }
            break;
        }
        std::array<char, IDSIZE> id = {};
        std::copy_n(buffer + 1, IDSIZE-1, id.begin());

        std::cout << std::string(id.begin(), std::find(id.begin(), id.end(), '\0')) << std::endl;

        infile.gets(buffer, BUFFERSIZE);

        // Process read
        std::string read(buffer);

        std::cout << read << std::endl;

        infile.gets(buffer, BUFFERSIZE);
        infile.gets(buffer, BUFFERSIZE);

        lcp::string* lcp = new lcp::string(read);

        lcp->deepen();
        lcp->deepen();

        // Hash the values
        std::vector<int> hashedRead;

        for( std::deque<lcp::core*>::iterator it = lcp->cores.begin(); it != lcp->cores.end(); it++ ) {
            std::cout << "hashing " << (*it)->label << std::endl;
            hashedRead.push_back( hash1((*it)->label) );
        }

        // Sort the hashed values
        std::sort(hashedRead.begin(), hashedRead.end());

        // Insert into Trie
        insert(root, hashedRead, id); // Using index as read ID

        break;
    }

    // Trie is now constructed reads

    delete root;

    return 0;
}
