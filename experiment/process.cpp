#ifndef PROCESS_CPP
#define PROCESS_CPP


#ifndef BUFFERSIZE
#define BUFFERSIZE      100000
#endif

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

#endif