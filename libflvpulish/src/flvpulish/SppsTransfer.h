#pragma once
#include "Transfer.h"

namespace record
{
    class SppsTransfer :
        public Transfer
    {
    public:
        SppsTransfer(void);
        ~SppsTransfer(void);
        virtual void transfer(Sample & sample);

    private:
        AVCDecoderConfigurationRecord video_conf_;
    };
}
