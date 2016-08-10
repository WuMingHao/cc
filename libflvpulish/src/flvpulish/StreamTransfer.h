#pragma once
#include "Transfer.h"

namespace record
{
    class StreamTransfer :
        public Transfer
    {
    public:
        StreamTransfer(void);
        ~StreamTransfer(void);
        virtual void transfer(Sample & sample);
    private:
        uint32_t paket_len_;
    };
}
