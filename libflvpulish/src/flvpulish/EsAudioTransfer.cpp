#include "EsAudioTransfer.h"

namespace record
{
    EsAudioTransfer::EsAudioTransfer(void)
    {
    }

    EsAudioTransfer::~EsAudioTransfer(void)
    {
    }

    void EsAudioTransfer::transfer(Sample & sample)
    {
        static AacTagHeader header;
        sample.data.push_front(const_buffer((uint8_t*)&header,sizeof(header)));
        sample.size += sizeof(header);
    }
}
