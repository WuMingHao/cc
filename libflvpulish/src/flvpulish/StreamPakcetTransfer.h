#pragma once
#include "Transfer.h"

namespace record
{
    class StreamPakcetTransfer :
        public Transfer
    {
    public:
        StreamPakcetTransfer(void);
        ~StreamPakcetTransfer(void);
        virtual void transfer(Sample & sample);
    private:
        uint32_t paket_len_;
    };
}
