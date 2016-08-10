#include "SppsTransfer.h"

namespace record
{
    static uint8_t nale_header[4] = {0x00,0x00,0x00,0x01}; 
    SppsTransfer::SppsTransfer(void)
    {
    }

    SppsTransfer::~SppsTransfer(void)
    {
    }

    void SppsTransfer::transfer(Sample & sample)
    {
        const_buffer& packet = *sample.data.begin();
        if(0 == memcmp(packet.data(),nale_header,4))
        {
            packet += 4;
            sample.size -= 4;
        }
         NaluHeader nalu;
        nalu.set(packet.data()[0]);

        if (nalu.nal_unit_type == NaluHeader::IDR)
        {
            sample.flags = Sample::sync;
        }
        else if (nalu.nal_unit_type == NaluHeader::UNIDR)
        {
		unsigned char daa = (unsigned char)packet.data()[1];
		daa = daa & 0xF0;
		if(daa == 0x80)
		{
			sample.flags = Sample::sync;
		}
	}
        else if (nalu.nal_unit_type == NaluHeader::SEI)
        {
            SliceHeader frametype(packet.data()[1]);
            if (frametype.type == SliceHeader::I)
            {
                sample.flags = Sample::sync;
            }
        }
        else if(nalu.nal_unit_type == NaluHeader::SPS 
            || nalu.nal_unit_type == NaluHeader::PPS)
        {
           
            if (nalu.nal_unit_type == NaluHeader::SPS)
            {
                video_conf_.set_sps(packet.data(),packet.length());
            }
            else
            {
                video_conf_.set_pps(packet.data(),packet.length());
            }
            sample.data.clear();
            sample.size = 0;

            if (sample.media_info && sample.media_info->format_data.size() < 1)
            {
                if (video_conf_.is_full())
                {
                    video_conf_.to_data(sample.media_info->format_data);
                }
            }
        }
    }
}
