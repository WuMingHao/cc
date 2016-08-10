#include "StreamPakcetTransfer.h"

namespace record
{
    static uint8_t nale_header[4] = {0x00,0x00,0x00,0x01}; 
    StreamPakcetTransfer::StreamPakcetTransfer(void)
        : paket_len_(0)
    {
    }

    StreamPakcetTransfer::~StreamPakcetTransfer(void)
    {
    }

    void StreamPakcetTransfer::transfer(Sample & sample)
    {
        const_buffer& packet = *sample.data.begin();
        if(0 == memcmp(packet.data(),nale_header,4))
        {
            packet += 4;
            sample.size -= 4;
        }
        paket_len_ = sample.size;
        local_to_bigendian((uint8_t *)&paket_len_,sizeof(uint32_t)); 

        sample.data.push_front(const_buffer((uint8_t *)&paket_len_,sizeof(uint32_t)));
        sample.size += sizeof(uint32_t);
    }
}
