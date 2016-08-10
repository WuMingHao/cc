#include "EsRtmpSink.h"
#include <iostream>

#include <rtmp/log.h>

namespace record
{
    EsRtmpSink::EsRtmpSink(const char* url)
        : rtmp_url_(url)
        , last_error_(0)
    {
        RTMP_LogSetLevel(RTMP_LOGCRIT);
        RTMP_Init(&rtmp_);
        char p_rtmp_url[256];
        memset(p_rtmp_url, 0, 256);
        memcpy(p_rtmp_url, rtmp_url_.c_str(), rtmp_url_.size());
        if (!RTMP_SetupURL(&rtmp_, p_rtmp_url)) {
            std::cout << "set url failed: url: " << rtmp_url_ << std::endl;
            last_error_ = -1;
        } else {
            RTMP_EnableWrite(&rtmp_);
            if (!RTMP_Connect(&rtmp_, NULL)) {
                std::cout << "rtmp connect failed" << std::endl;
                last_error_ = -1;
            } else if (!RTMP_ConnectStream(&rtmp_, 0)) {
                std::cout << "rtmp connect stream failed" << std::endl;
                last_error_ = -1;
            }
        }
    }

    EsRtmpSink::~EsRtmpSink(void)
    {
        RTMP_Close(&rtmp_);
        rtmp_url_.clear();
    }

    uint32_t EsRtmpSink::write(Sample& sample)
    {
        uint32_t frame_size = 0; //Ä¬ÈÏÎªÊ§°Ü

        if (last_error_) 
        {
        } 
        else if (!RTMP_IsConnected(&rtmp_)) 
        {
             std::cout << "rtmp connect failed" << std::endl;
        } 
        else if (RTMP_IsTimedout(&rtmp_)) 
        {
             std::cout << "rtmp time out" << std::endl;
        }
        else
        {
            

            RTMPPacket rtmp_packet;
            RTMPPacket_Reset(&rtmp_packet);
            RTMPPacket_Alloc(&rtmp_packet,sample.size);

            rtmp_packet.m_packetType = (sample.media_info)?sample.media_info->type:0x12;
            rtmp_packet.m_nBodySize = sample.size;
            rtmp_packet.m_nTimeStamp = sample.time;
            rtmp_packet.m_nChannel = 4;
            rtmp_packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
            rtmp_packet.m_nInfoField2 = rtmp_.m_stream_id; 


            for (std::deque<const_buffer>::iterator iter = sample.data.begin(); 
                iter != sample.data.end();
                iter++) {
                    uint8_t *p = (*iter).data();
                    uint32_t size = (*iter).length();
                    memcpy(rtmp_packet.m_body + frame_size, p, size);
                    frame_size += size;
            }

           // memcpy(rtmp_packet.m_body,buffer_,frame_size);

            if (!RTMP_SendPacket(&rtmp_,&rtmp_packet,0))
            {
                frame_size = 0;
            }
            
            RTMPPacket_Free(&rtmp_packet);
        }
        return frame_size;
    }
}
