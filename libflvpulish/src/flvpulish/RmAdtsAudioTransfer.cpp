#include "RmAdtsAudioTransfer.h"

namespace record
{
    RmAdtsAudioTransfer::RmAdtsAudioTransfer(void)
    {
    }

    RmAdtsAudioTransfer::~RmAdtsAudioTransfer(void)
    {
    }

    void RmAdtsAudioTransfer::transfer(Sample & sample)
    {
        const_buffer& packet = *sample.data.begin();
        if(packet.length() > 7 && check_has_adts(packet.data()))
        {
            packet += 7;
            sample.size -= 7;
        }
    }

    bool RmAdtsAudioTransfer::check_has_adts(const uint8_t* stream)
    {
        return (stream[0] == 0xff && (stream[1]>>4) == 0xf);
    }
}
