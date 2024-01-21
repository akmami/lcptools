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