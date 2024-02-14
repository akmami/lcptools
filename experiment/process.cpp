#ifndef PROCESS_CPP
#define PROCESS_CPP


#ifndef BUFFERSIZE
#define BUFFERSIZE      100000
#endif

#include <string>
#include <algorithm>

/**
 * @brief Processes a string by removing specific characters and checks for the presence of a character.
 *
 * This function modifies the given character array in place. It removes all '-' characters
 * from the array up to the first newline character '\n' or until BUFFERSIZE is reached. 
 * Additionally, it checks for the presence of the character 'N' in the string. The function 
 * sets a flag to true if 'N' is found at any position in the string.
 *
 * This function is particularly useful in genomic data processing, where '-' might represent 
 * a deletion or a gap in an alignment that needs to be removed. The presence of 'N' in the 
 * sequence might signify a specific condition or marker in the sequence that requires 
 * special handling or attention.
 *
 * @param str A character array of size BUFFERSIZE. The array is modified in place by removing 
 *        '-' characters, and the function ensures the processed string is null-terminated.
 * @return Returns true if the character 'N' is found in the string; otherwise, false.
 */
bool process( char *str ) {
    
    size_t writeIndex = 0, readIndex = 0;
    bool valid = true;

    for ( ; readIndex < BUFFERSIZE && str[readIndex] != '\n' && str[readIndex] != '\0'; readIndex++ ) {

        if ( str[readIndex] != '-' ) {
            str[writeIndex++] = str[readIndex];
        }

        if ( str[readIndex] == 'N' ) {
            valid = false;
        }
    }

    str[writeIndex] = '\0';

    return valid;
};

/**
 * @brief Processes a string by removing specific characters and checks for validity.
 *
 * This function iterates over the given string, removing any '-' characters found and
 * simultaneously checking for the presence of 'N' characters. The input string is modified
 * in place, with '-' characters removed. The presence of 'N' signifies that the string
 * is not valid for certain applications, such as genomic data processing where 'N' might
 * represent an unknown nucleotide.
 *
 * @param str A reference to the string to be processed. The string is modified in place,
 *            with '-' characters removed.
 * @return Returns true if the string does not contain any 'N' characters, indicating it
 *         is valid. Returns false if at least one 'N' character is found.
 */
bool process( std::string& str ) {
    size_t writeIndex = 0;
    bool valid = true;

    for (size_t readIndex = 0; readIndex < str.length(); readIndex++) {
        if (str[readIndex] != '-') {
            str[writeIndex++] = str[readIndex];
        }

        if (str[readIndex] == 'N') {
            valid = false;
        }
    }

    str.resize(writeIndex); // Adjust the size of the string to the new length

    return valid;
};

/**
 * @brief Generates the reverse complement of a DNA sequence.
 *
 * This function takes a DNA sequence as input, reverses it, and then replaces each
 * nucleotide with its complement (A <-> T, C <-> G). This operation is commonly used
 * in bioinformatics for DNA sequence analysis, especially when working with palindromic
 * sequences or when preparing to align sequence reads from both strands.
 *
 * @param sequence The DNA sequence to be reversed and complemented.
 * @return Returns a new string containing the reverse complement of the input sequence.
 */
bool reverseComplement( std::string& sequence ) {
    
    // reverse the sequence
    std::reverse( sequence.begin(), sequence.end() );
    
    // replace each nucleotide with its complement
    std::transform( sequence.begin(), sequence.end(), sequence.begin(),
                   [](char nucleotide) -> char {
                       switch (nucleotide) {
                           case 'A': return 'T';
                           case 'T': return 'A';
                           case 'G': return 'C';
                           case 'C': return 'G';
                           default: return nucleotide;
                       }
                   });
    
    return true;
}


#endif