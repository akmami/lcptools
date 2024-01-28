#ifndef GZFILE_HPP
#define GZFILE_HPP

#include <zlib.h>

#ifndef BUFFERSIZE
#define BUFFERSIZE      100000
#endif


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

    // Rewind file
    void rewind(){
        gzrewind(gzFile_);
    }

    // Print to compressed file
    int printf(const char* format, ...) {
        char buffer[BUFFERSIZE];
        va_list args;
        va_start(args, format);
        int ret = vsnprintf(buffer, BUFFERSIZE, format, args);
        va_end(args);
        if (ret > 0 && ret < BUFFERSIZE) {
            gzwrite(gzFile_, buffer, ret);
        }
        return ret;
    }

    // Check if the file is valid
    explicit operator bool() const {
        return gzFile_ != nullptr;
    }

private:
    gzFile gzFile_;
};


#endif