#pragma once
#include "Transfer.h"

namespace record
{
    class FlvAudioTransfer : public Transfer
    {
    public:
        FlvAudioTransfer(void);
        ~FlvAudioTransfer(void);

        virtual void transfer(Sample & sample);
    };
}
