#pragma once
#include "MuxerBase.h"

namespace record
{
    class Sink
    {
    public:

        static Sink* create(const char* url, const char* type);

		bool bSuccess;

        Sink(void);
        virtual ~Sink(void);

        virtual uint32_t write(Sample& sample) = 0;
    };
}
