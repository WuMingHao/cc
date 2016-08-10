#pragma once

#include "Sink.h"
#include <string>
#include <rtmp/rtmp.h>

namespace record
{
    class FlvRtmpSink : public Sink
    {
    public:
        FlvRtmpSink(const char* url);
        ~FlvRtmpSink(void);

        uint32_t write(Sample& sample);
    private:
        std::string rtmp_url_;
        char * buffer_;
        RTMP rtmp_;

        uint32_t last_error_;
    };
}
