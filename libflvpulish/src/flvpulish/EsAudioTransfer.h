#pragma once
#include "Transfer.h"

namespace record
{
    class EsAudioTransfer : public Transfer
    {
    public:
        EsAudioTransfer(void);
        ~EsAudioTransfer(void);

        virtual void transfer(Sample & sample);
    };
}
