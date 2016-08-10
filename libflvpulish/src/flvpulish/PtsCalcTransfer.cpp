#include "PtsCalcTransfer.h"

namespace record
{
    static uint8_t nale_header[4] = {0x00,0x00,0x00,0x01}; 
    PtsCalcTransfer::PtsCalcTransfer(void)
        : paket_len_(0)
        , m_file(NULL)
    {
        //m_file = fopen("d://spps.txt","wb");
    }

    PtsCalcTransfer::~PtsCalcTransfer(void)
    {
        if(m_file)
        {
            fclose(m_file);
            m_file = NULL;
        }
    }

    void PtsCalcTransfer::write(uint8_t* w, size_t s)
    {
        if(m_file)
        {
            fwrite(w,1,s,m_file);
            fwrite("\r\n",1,2,m_file);
        }
    }

    void PtsCalcTransfer::transfer(Sample & sample)
    {
        const_buffer& packet = *sample.data.begin();
        if(0 == memcmp(packet.data(),nale_header,4))
        {
            packet += 4;
            sample.size -= 4;
        }
        NaluHeader nalu;
        nalu.set(packet.data()[0]);

        paket_len_ = sample.size;
        local_to_bigendian((uint8_t *)&paket_len_,sizeof(uint32_t)); 

        sample.data.push_front(const_buffer((uint8_t *)&paket_len_,sizeof(uint32_t)));
        sample.size += sizeof(uint32_t);

        if (nalu.nal_unit_type == NaluHeader::IDR)
        {
            sample.flags = Sample::sync;
            write((uint8_t*)"[IDR]\r\n",7);
            write(packet.data(),4);
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
           
            if (nalu.nal_unit_type == NaluHeader::SPS)
            {
                 write((uint8_t*)"[SPS]\r\n",7);
                video_conf_.set_sps(packet.data(),packet.length());
            }
            else
            {
                write((uint8_t*)"[PPS]\r\n",7);
                video_conf_.set_pps(packet.data(),packet.length());
            }
             write(packet.data(),packet.length());

            if (sample.media_info && sample.media_info->format_data.size() < 1)
            {
                sample.data.clear();
                sample.size = 0;

                if (video_conf_.is_full())
                {
                    video_conf_.to_data(sample.media_info->format_data);
                }
            }
        }
    }
}
