#ifndef GZFILE_HPP
#define GZFILE_HPP

#define IDSIZE      128

struct MafRead {
    char id[IDSIZE];
    size_t genome_num;
    long pos;
    long length;
};

#endif