/**
 * @file    MafRead.hpp
 * @brief   Definition of MafRead Structure
 *
 * This header file contains the definition of the MafRead structure, which is
 * designed to represent individual reads extracted from MAF (Multiple Alignment
 * Format) files. Each MafRead contains an identifier (id), genome number (genome_num),
 * position in the genome (pos), and the length of the read (length).
 *
 * The MafRead structure is utilized in genomic data processing, specifically for
 * handling and analyzing data from MAF files. The IDSIZE constant defines the maximum
 * size of the read identifier, ensuring consistent handling of identifiers across the
 * application.
 *
 * Usage Example:
 *     MafRead read;
 *     strncpy(read.id, "read_id", IDSIZE);
 *     read.genome_num = 1;
 *     read.pos = 100;
 *     read.length = 50;
 *     // Use the read struct as required...
 */


#ifndef MAFREAD_HPP
#define MAFREAD_HPP

#define IDSIZE      128

struct MafRead {
    char id[IDSIZE];
    size_t genome_num;
    long pos;
    long length;
};

#endif