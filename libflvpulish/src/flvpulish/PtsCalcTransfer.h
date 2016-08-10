#pragma once
#include "Transfer.h"

#include <stdio.h>

namespace record
{
    class PtsCalcTransfer :
        public Transfer
    {
    public:
        PtsCalcTransfer(void);
        ~PtsCalcTransfer(void);
        virtual void transfer(Sample & sample);
    private:
        void write(uint8_t* w, size_t s);
    private:
        uint32_t paket_len_;
        AVCDecoderConfigurationRecord video_conf_;
        FILE* m_file;
    };
}
