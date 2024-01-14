#ifndef GZFILE_HPP
#define GZFILE_HPP

#include <zlib.h>

#define BUFFERSIZE  100000
#define IDSIZE      128

class GzFile {
public:
    GzFile(const char* filename, const char* mode) {
        gzFile_ = gzopen(filename, mode);
    }

    ~GzFile() {
        if (gzFile_) gzclose(gzFile_);
    }

    // Returns true if end of file has been reached
    bool eof() const {
        return gzFile_ && gzeof(gzFile_);
    }

    // Wrapper method for gzgets
    char* gets(char* buffer, int len) {
        return gzFile_ ? gzgets(gzFile_, buffer, len) : Z_NULL;
    }

    // Check if the file is valid
    explicit operator bool() const {
        return gzFile_ != nullptr;
    }

private:
    gzFile gzFile_;
};


#endif