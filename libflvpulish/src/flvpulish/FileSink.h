#pragma once
#include "Sink.h"
#include <stdio.h>
namespace record
{
    class FileSink :
        public Sink
    {
    public:
        FileSink(const char* url);
        ~FileSink(void);

        uint32_t write(Sample& sample);
    private:
        FILE* file_;
    };
}
