#include "FlvAudioTransfer.h"

namespace record
{
    FlvAudioTransfer::FlvAudioTransfer(void)
    {
    }

    FlvAudioTransfer::~FlvAudioTransfer(void)
    {
    }

    void FlvAudioTransfer::transfer(Sample & sample)
    {
        AudioTag audioHeader;
        audioHeader.date_size = sample.size+2;
        audioHeader.timestample = sample.time;
        audioHeader.OArchive();
        
        sample.data.push_front(const_buffer((uint8_t *)&audioHeader,sizeof(AudioTag),true));
        sample.size += sizeof(AudioTag);

        uint32_t pre_size = sample.size;
        local_to_bigendian((uint8_t *)&pre_size,sizeof(uint32_t)); 

        sample.data.push_back(const_buffer((uint8_t *)&pre_size,sizeof(uint32_t),true));
        sample.size += sizeof(uint32_t);
    }
}
