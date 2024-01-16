#ifndef GZFILE_HPP
#define GZFILE_HPP

#define IDSIZE      128

struct MafRead {
    char id[IDSIZE];
    size_t genome_num;
    long pos;
    long length;
};


bool mr_cmpr(const MafRead& a, const MafRead& b) {
    return a.genome_num < b.genome_num || ( a.genome_num == b.genome_num && a.pos < b.pos);
};

#endif