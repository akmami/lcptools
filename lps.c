#include "lps.h"

void reverse(const char *str, int len, char **rev) {
    *rev = (char*) malloc(len*sizeof(char));
    int left = 0;
    int right = len - 1;

    while (left < right) {
        (*rev)[left] = str[right];
        (*rev)[right] = str[left];

        left++;
        right--;
    }
    if (left == right) {
        (*rev)[left] = str[left];
    }
}

void init_lps(struct lps *nlps, const char *str, int len) {   
    nlps->level = 1;
    nlps->size = 0;
    nlps->cores = (struct core *)malloc((len/CONSTANT_FACTOR)*sizeof(struct core));
    nlps->size = parse1(str, str+len, nlps->cores, 0);
}

void init_lps_offset(struct lps *nlps, const char *str, int len, uint64_t offset) {   
    nlps->level = 1;
    nlps->size = 0;
    nlps->cores = (struct core *)malloc((len/CONSTANT_FACTOR)*sizeof(struct core));
    nlps->size = parse1(str, str+len, nlps->cores, offset);
}

void init_lps2(struct lps *nlps, const char *str, int len) {   
    nlps->level = 1;
    nlps->size = 0;
    nlps->cores = (struct core *)malloc((len/CONSTANT_FACTOR)*sizeof(struct core));
    char *rev = NULL;
    reverse(str, len, &rev);
    nlps->size = parse2(rev, rev+len, nlps->cores, 0);
    free(rev);
}

void init_lps3(struct lps *nlps, FILE *in) {
    // read the level from the binary file
    if (fread(&(nlps->level), sizeof(int), 1, in) != 1) {
        fprintf(stderr, "Error reading level from file\n");
        exit(EXIT_FAILURE);
    }

    // read the size (number of cores)
    if(fread(&(nlps->size), sizeof(int), 1, in) != 1) {
        fprintf(stderr, "Error reading size from file\n");
        exit(EXIT_FAILURE);
    }

    nlps->cores = NULL;

    if (nlps->size) {
        // allocate memory for the cores array
        nlps->cores = (struct core *)malloc(nlps->size * sizeof(struct core));

        // read each core object from the file
        for (int i = 0; i < nlps->size; i++) {
            struct core *cr = &(nlps->cores[i]);

            if (fread(&(cr->bit_size), sizeof(ubit_size), 1, in) != 1) {
                fprintf(stderr, "Error reading bit_size from file at %d\n", i);
                exit(EXIT_FAILURE);
            }
    
            ubit_size block_number = (cr->bit_size + UBLOCK_BIT_SIZE - 1) / UBLOCK_BIT_SIZE;
            cr->bit_rep = (ublock *)malloc(block_number * sizeof(ublock));
            if (fread(cr->bit_rep, block_number * sizeof(ublock), 1, in) != 1) {
                fprintf(stderr, "Error reading bit_rep from file at %d\n", i);
                exit(EXIT_FAILURE);
            }
         
            if (fread(&(cr->label), sizeof(ulabel), 1, in) != 1) {
                fprintf(stderr, "Error reading label from file at %d\n", i);
                exit(EXIT_FAILURE);
            }
            if (fread(&(cr->start), sizeof(uint64_t), 1, in) != 1) {
                fprintf(stderr, "Error reading start from file at %d\n", i);
                exit(EXIT_FAILURE);
            }
            if (fread(&(cr->end), sizeof(uint64_t), 1, in) != 1) {
                fprintf(stderr, "Error reading end from file at %d\n", i);
                exit(EXIT_FAILURE);
            }
        }
    }
}

void init_lps4(struct lps *nlps, const char *str, int len, int lcp_level, int sequence_split_length) {

    if (lcp_level < 1)
        return;

    nlps->level = 1;
    nlps->size = 0; 
    int estimated_size = (int)(len / pow( (double)CONSTANT_FACTOR, lcp_level));
    nlps->cores = (struct core *)malloc(estimated_size*sizeof(struct core));

    int index = 0, core_index = 0, proceed = 1;

    while (index < len && proceed) {
        int str_len = minimum(sequence_split_length, len-index);
        struct lps temp_lps;
        init_lps_offset(&temp_lps, str+index, str_len, index);
        lps_deepen(&temp_lps, lcp_level);
        
        if (temp_lps.size>1) {
            int overlap = 0;
            for (; overlap<temp_lps.size; overlap++) {
                if (temp_lps.cores[overlap].start == nlps->cores[core_index-1].start) {
                    memcpy(nlps->cores+core_index, temp_lps.cores+overlap+1, (temp_lps.size-overlap-1)*sizeof(struct core));
                    core_index += (temp_lps.size-overlap-1);
                    nlps->size += (temp_lps.size-overlap-1);
                    index = nlps->cores[core_index-3].start+1;
                    free(temp_lps.cores);
                    break;
                }
            }
            if ( overlap==temp_lps.size) {
                memcpy(nlps->cores+core_index, temp_lps.cores, temp_lps.size*sizeof(struct core));
                core_index += temp_lps.size;
                nlps->size += temp_lps.size;
                index = nlps->cores[core_index-3].start+1;
                free(temp_lps.cores);
            }
        } else {
            break;
        }

        proceed = (str_len == sequence_split_length);
    }
}

void free_lps(struct lps *nlps) {
    for(int i=0; i<nlps->size; i++) {
        free(nlps->cores[i].bit_rep);
    }
    free(nlps->cores);
    nlps->size = 0;
}

void write_lps(struct lps *nlps, FILE *out) {
    // write the level field
    fwrite(&(nlps->level), sizeof(int), 1, out);

    // write the size (number of cores)
    fwrite(&(nlps->size), sizeof(int), 1, out);

    // write each core object iteratively
    if (nlps->size) {
        for (int i = 0; i < nlps->size; i++) {
            const struct core *cr = &(nlps->cores[i]);

            fwrite(&(cr->bit_size), sizeof(ubit_size), 1, out);
            
            ubit_size block_number = (cr->bit_size + UBLOCK_BIT_SIZE - 1) / UBLOCK_BIT_SIZE;
            fwrite(cr->bit_rep, sizeof(ublock), block_number, out);
            
            fwrite(&(cr->label), sizeof(ulabel), 1, out);
            fwrite(&(cr->start), sizeof(uint64_t), 1, out);
            fwrite(&(cr->end), sizeof(uint64_t), 1, out);
        }
    }
}

int parse1(const char *begin, const char *end, struct core *cores, uint64_t offset) {

    const char *it1 = begin;
    const char *it2 = end;
    int core_index = 0;

    // find lcp cores
    for (; it1 + 2 < end; it1++) {

        // skip invalid character
        if (alphabet[(unsigned char)*it1] == alphabet[(unsigned char)*(it1+1)]) {
            continue;
        }

        // check for RINT core
        if (alphabet[(unsigned char)*(it1+1)] == alphabet[(unsigned char)*(it1+2)]) {

            // count middle characters
            uint32_t middle_count = 1;
            const char *temp = it1 + 2;
            while (temp < end && alphabet[(unsigned char)*(temp-1)] == alphabet[(unsigned char)*temp]) {
                temp++;
                middle_count++;
            }
            if (temp != end) {
                // check if there is any SSEQ cores left behind
                if (it2 < it1) {
                    init_core1(&(cores[core_index]), it2-1, it1-it2+2, it2-begin-1+offset, it1-begin+1+offset);
                    core_index++;
                }

                // create RINT core
                it2 = it1 + 2 + middle_count;
                init_core1(&(cores[core_index]), it1, it2-it1, it1-begin+offset, it2-begin+offset);
                core_index++;

                continue;
            }
        }

        if (alphabet[(unsigned char)*it1] > alphabet[(unsigned char)*(it1+1)] &&
            alphabet[(unsigned char)*(it1+1)] < alphabet[(unsigned char)*(it1+2)] ) {

            // check if there is any SSEQ cores left behind
            if (it2 < it1) {
                init_core1(&(cores[core_index]), it2-1, it1-it2+2, it2-begin-1+offset, it1-begin+1+offset);
                core_index++;
            }

            // create LMIN core
            it2 = it1 + 3;
            init_core1(&(cores[core_index]), it1, it2-it1, it1-begin+offset, it2-begin+offset);
            core_index++;

            continue;
        }

        if (begin == it1) {
            continue;
        }

        // check for LMAX
        if (it1+3 < end &&
            alphabet[(unsigned char)*it1] < alphabet[(unsigned char)*(it1+1)] &&
            alphabet[(unsigned char)*(it1+1)] > alphabet[(unsigned char)*(it1+2)] &&
            alphabet[(unsigned char)*(it1-1)] <= alphabet[(unsigned char)*(it1)] &&
            alphabet[(unsigned char)*(it1+2)] >= alphabet[(unsigned char)*(it1+3)] ) {

            // check if there is any SSEQ cores left behind
            if (it2 < it1) {
                init_core1(&(cores[core_index]), it2-1, it1-it2+2, it2-begin-1+offset, it1-begin+1+offset);
                core_index++;
            }

            // create LMAX core
            it2 = it1 + 3;
            init_core1(&(cores[core_index]), it1, it2-it1, it1-begin+offset, it2-begin+offset);
            core_index++;

            continue;
        }
    }

    return core_index;
}

int parse2(const char *begin, const char *end, struct core *cores, uint64_t offset) {

    const char *it1 = begin;
    const char *it2 = end;
    int core_index = 0;

    // find lcp cores
    for (; it1 + 2 < end; it1++) {

        // skip invalid character
        if (rc_alphabet[(unsigned char)*it1] == rc_alphabet[(unsigned char)*(it1+1)]) {
            continue;
        }

        // check for RINT core
        if (rc_alphabet[(unsigned char)*(it1+1)] == rc_alphabet[(unsigned char)*(it1+2)]) {

            // count middle characters
            uint32_t middle_count = 1;
            const char *temp = it1 + 2;
            while (temp < end && rc_alphabet[(unsigned char)*(temp-1)] == rc_alphabet[(unsigned char)*temp]) {
                temp++;
                middle_count++;
            }
            if (temp != end) {
                // check if there is any SSEQ cores left behind
                if (it2 < it1) {
                    init_core2(&(cores[core_index]), it2-1, it1-it2+2, it2-begin-1+offset, it1-begin+1+offset);
                    core_index++;
                }

                // create RINT core
                it2 = it1 + 2 + middle_count;
                init_core2(&(cores[core_index]), it1, it2-it1, it1-begin+offset, it2-begin+offset);
                core_index++;

                continue;
            }
        }

        if (rc_alphabet[(unsigned char)*it1] > rc_alphabet[(unsigned char)*(it1+1)] &&
            rc_alphabet[(unsigned char)*(it1+1)] < rc_alphabet[(unsigned char)*(it1+2)] ) {

            // check if there is any SSEQ cores left behind
            if (it2 < it1) {
                init_core2(&(cores[core_index]), it2-1, it1-it2+2, it2-begin-1+offset, it1-begin+1+offset);
                core_index++;
            }

            // create LMIN core
            it2 = it1 + 3;
            init_core2(&(cores[core_index]), it1, it2-it1, it1-begin+offset, it2-begin+offset);
            core_index++;

            continue;
        }

        if (begin == it1) {
            continue;
        }

        // check for LMAX
        if (it1+3 < end &&
            rc_alphabet[(unsigned char)*it1] < rc_alphabet[(unsigned char)*(it1+1)] &&
            rc_alphabet[(unsigned char)*(it1+1)] > rc_alphabet[(unsigned char)*(it1+2)] &&
            rc_alphabet[(unsigned char)*(it1-1)] <= rc_alphabet[(unsigned char)*(it1)] &&
            rc_alphabet[(unsigned char)*(it1+2)] >= rc_alphabet[(unsigned char)*(it1+3)] ) {

            // check if there is any SSEQ cores left behind
            if (it2 < it1) {
                init_core2(&(cores[core_index]), it2-1, it1-it2+2, it2-begin-1+offset, it1-begin+1+offset);
                core_index++;
            }

            // create LMAX core
            it2 = it1 + 3;
            init_core2(&(cores[core_index]), it1, it2-it1, it1-begin+offset, it2-begin+offset);
            core_index++;

            continue;
        }
    }

    return core_index;
}

int parse3(struct core *begin, struct core *end, struct core *cores) {

    struct core *it1 = begin + DCT_ITERATION_COUNT;
    struct core *it2 = end;
    int core_index = 0;

    // find lcp cores
    for (; it1 + 2 < end; it1++) {

        // skip invalid character
        if (core_eq(it1, it1+1)) {
            continue;
        }

        // check for RINT core
        if (core_eq(it1+1, it1+2)) {

            // count middle characters
            uint32_t middle_count = 1;
            struct core *temp = it1 + 2;
            while (temp < end && core_eq(temp-1, temp)) {
                temp++;
                middle_count++;
            }
            if (temp != end) {
                // check if there is any SSEQ cores left behind
                if (it2 < it1) {
                    init_core3(&(cores[core_index]), it2-1-DCT_ITERATION_COUNT, it1-it2+2+DCT_ITERATION_COUNT);
                    core_index++;
                }

                // create RINT core
                it2 = it1 + 2 + middle_count;
                init_core3(&(cores[core_index]), it1-DCT_ITERATION_COUNT, it2-it1+DCT_ITERATION_COUNT);
                core_index++;

                continue;
            }
        }

        // check for LMIN
        if (core_gt(it1, it1+1) && core_lt(it1+1, it1+2)) {
            
            // check if there is any SSEQ cores left behind
            if (it2 < it1) {
                init_core3(&(cores[core_index]), it2-1-DCT_ITERATION_COUNT, it1-it2+2+DCT_ITERATION_COUNT);
                core_index++;
            }

            // create LMIN core
            it2 = it1 + 3;
            init_core3(&(cores[core_index]), it1-DCT_ITERATION_COUNT, it2-it1+DCT_ITERATION_COUNT);
            core_index++;

            continue;
        }

        if (begin == it1) {
            continue;
        }

        // check for LMAX
        if (it1+3 < end &&
            core_lt(it1, it1+1) &&
            core_gt(it1+1, it1+2) &&
            core_leq(it1-1, it1) &&
            core_geq(it1+2, it1+3)) {

            // check if there is any SSEQ cores left behind
            if (it2 < it1) {
                init_core3(&(cores[core_index]), it2-1-DCT_ITERATION_COUNT, it1-it2+2+DCT_ITERATION_COUNT);
                core_index++;
            }

            // create LMAX core
            it2 = it1 + 3;
            init_core3(&(cores[core_index]), it1-DCT_ITERATION_COUNT, it2-it1+DCT_ITERATION_COUNT);
            core_index++;

            continue;
        }
    }
    return core_index;
}

int64_t lps_memsize(const struct lps *str) {
    uint64_t total = sizeof(struct lps);
    
    for(int i=0; i<str->size; i++) {
        total += core_memsize(&(str->cores[i]));
    }

    return total;
}

/**
 * @brief Performs Deterministic Coin Tossing (DCT) compression on binary sequences.
 *
 * This function is a central part of the LCP (Locally Consisted Parsing) algorithm. It identifies differences
 * between consecutive binary strings, compressing the information by focusing on the position and value of
 * the first divergent bit from the right-end of the strings. This difference is used to generate a compact
 * 'core' that encapsulates the unique elements of each sequence in a smaller binary form.
 *
 * This compression significantly reduces redundant information, making further analysis of the sequences
 * within the LCP framework more efficient and manageable.
 *
 * @return 0 if dct is performed, -1 if no enough cores are available for dct.
 */
int lcp_dct(struct lps *str) {

    // at least 2 cores are needed for compression
    if (str->size < DCT_ITERATION_COUNT + 1) {
        return -1;
    }

    for (uint64_t dct_index = 0; dct_index < DCT_ITERATION_COUNT; dct_index++) {
        struct core *it_left = str->cores + str->size - 2, *it_right = str->cores + str->size - 1;

        for (; str->cores + dct_index <= it_left; it_left--, it_right--) {
            core_compress(it_left, it_right);
        }
    }

    return 0;
}

int lps_deepen1(struct lps *str) {

    // compress cores
    if (lcp_dct(str) < 0) {
        for(int i=0; i<str->size; i++) {
            free(str->cores[i].bit_rep);
        }
        str->size = 0;
        str->level++;
        return 0;
    }

    // find new cores
    int new_size = parse3(str->cores + DCT_ITERATION_COUNT, str->cores + str->size, str->cores);
    int temp = new_size;

    // remove old cores
    while(temp < str->size) {
        free(str->cores[temp].bit_rep);
        temp++;
    }
    str->size = new_size;

    str->level++;

    if (str->size)
        str->cores = (struct core*)realloc(str->cores, str->size * sizeof(struct core));

    return 1;
}

int lps_deepen(struct lps *str, int lcp_level) {

    if (lcp_level <= str->level)
        return 0;

    while (str->level < lcp_level && lps_deepen1(str))
        ;

    return 1;
}

void print_lps(const struct lps *str) {
    printf("Level: %d \n", str->level);
    for(int i=0; i<str->size; i++) {
        print_core(&(str->cores[i]));
        printf(" ");
    }
}

int lps_eq(const struct lps *lhs, const struct lps *rhs) {
    if (lhs->size != rhs->size) {
        return 0;
    }

    for(int i=0; i<lhs->size; i++) {
        if (core_neq(&(lhs->cores[i]), &(rhs->cores[i])) != 0) {
            return 0;
        }
    }

    return 1;
}

int lps_neq(const struct lps *lhs, const struct lps *rhs) {
    if (lhs->size != rhs->size) {
        return 1;
    }

    for(int i=0; i<lhs->size; i++) {
        if (core_neq(&(lhs->cores[i]), &(rhs->cores[i])) != 0) {
            return 1;
        }
    }

    return 0;
}