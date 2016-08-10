#include "FlvRtmpSink.h"
#include <iostream>

#include <rtmp/log.h>


 FILE *fpLog = NULL;
namespace record
{
    static const uint32_t g_bufer_size =  1*1024*1024;
    FlvRtmpSink::FlvRtmpSink(const char* url)
        : rtmp_url_(url)
        , last_error_(0)
    {
		buffer_ = new char[g_bufer_size];
        fpLog = fopen("d:\\rtmplog.txt","a");
        if(fpLog != NULL)
        {
		   RTMP_LogSetOutput(fpLog);
        }

        RTMP_LogSetLevel(RTMP_LOGCRIT);
        RTMP_Log(RTMP_LOGERROR, "---------分割线-------");
        RTMP_Init(&rtmp_);
        char p_rtmp_url[256];
        memset(p_rtmp_url, 0, 256);
        memcpy(p_rtmp_url, rtmp_url_.c_str(), rtmp_url_.size());
        if (!RTMP_SetupURL(&rtmp_, p_rtmp_url)) {
            std::cout << "set url failed: url: " << rtmp_url_ << std::endl;
            RTMP_Log(RTMP_LOGERROR, "set url failed:");
            last_error_ = -1;
        } else {
            RTMP_EnableWrite(&rtmp_);
            if (!RTMP_Connect(&rtmp_, NULL)) {
                RTMP_Log(RTMP_LOGERROR, "rtmp connect failed:");
                last_error_ = -1;
            } else if (!RTMP_ConnectStream(&rtmp_, 0)) {
                 RTMP_Log(RTMP_LOGERROR, "rtmp connect stream failed failed:");
                last_error_ = -1;
            }
        }

        if(last_error_ == 0)
        bSuccess = true;
    }

    FlvRtmpSink::~FlvRtmpSink(void)
    {
        if(fpLog != NULL)
        {
            fclose(fpLog);
        }
        RTMP_Close(&rtmp_);

        delete [] buffer_;
        buffer_ = NULL;

        rtmp_url_.clear();
    }

    uint32_t FlvRtmpSink::write(Sample& sample)
    {
        uint32_t frame_size = 0; //默认为失败

        if (last_error_) 
        {
        } 
        else if (!RTMP_IsConnected(&rtmp_)) 
        {
              RTMP_Log(RTMP_LOGERROR, "rtmp connect failed RTMP_IsConnected");
        } 
        else if (RTMP_IsTimedout(&rtmp_)) 
        {
             RTMP_Log(RTMP_LOGERROR, "rtmp time out RTMP_IsConnected");
        }
        else
        {
            for (std::deque<const_buffer>::iterator iter = sample.data.begin(); 
                iter != sample.data.end();
                iter++) {
                    uint8_t *p = (*iter).data();
                    uint32_t size = (*iter).length();
                    memcpy(buffer_ + frame_size, p, size);
                    frame_size += size;
            }

            if (-1 ==  RTMP_Write(&rtmp_, buffer_, frame_size))
            {
                frame_size = 0;
            }
           
        }
        return frame_size;
    }
}
