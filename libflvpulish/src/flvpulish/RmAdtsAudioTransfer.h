#pragma once
#include "Transfer.h"

namespace record
{
    class RmAdtsAudioTransfer : public Transfer
    {
    public:
        RmAdtsAudioTransfer(void);
        ~RmAdtsAudioTransfer(void);

        virtual void transfer(Sample & sample);
    private:
        bool check_has_adts(const uint8_t* stream);
    };
}
