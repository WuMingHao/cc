#include "StreamTransfer.h"

namespace record
{
    static uint8_t nale_header[4] = {0x00,0x00,0x00,0x01}; 
    StreamTransfer::StreamTransfer(void)
        : paket_len_(0)
    {

    }

    StreamTransfer::~StreamTransfer(void)
    {

    }

    void StreamTransfer::transfer(Sample & sample)
    {
        const_buffer& packet = *sample.data.begin();
        if(0 == memcmp(packet.data(),nale_header,4))
        {
            packet += 4;
            sample.size -= 4;
        }
        NaluHeader nalu;
        nalu.set(packet.data()[0]);

        H264TagHeader header;

        if (nalu.nal_unit_type == NaluHeader::IDR)
        {
            sample.flags = Sample::sync;
        }
        else if (nalu.nal_unit_type == NaluHeader::UNIDR)
        {

        }
        else if (nalu.nal_unit_type == NaluHeader::SEI)
        {
            sample.flags = Sample::sync;
        }
        else if(nalu.nal_unit_type == NaluHeader::SPS 
            || nalu.nal_unit_type == NaluHeader::PPS)
        {

        }

        if (sample.flags == Sample::sync)
            header.FrameType = 1;
        else
            header.FrameType = 2;
        header.CompositionTime = sample.cts_delta; 
        header.OArchive();

        paket_len_ = sample.size;
        local_to_bigendian((uint8_t *)&paket_len_,sizeof(uint32_t)); 

        sample.data.push_front(const_buffer((uint8_t *)&paket_len_,sizeof(uint32_t)));
        sample.size += sizeof(uint32_t);

        sample.data.push_front(const_buffer((uint8_t *)&header,sizeof(header),true));
        sample.size += sizeof(header);
    }
}
