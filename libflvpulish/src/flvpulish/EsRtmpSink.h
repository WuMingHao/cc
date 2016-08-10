#pragma once

#include "Sink.h"
#include <string>
#include <rtmp/rtmp.h>

namespace record
{
    class EsRtmpSink : public Sink
    {
    public:
        EsRtmpSink(const char* url);
        ~EsRtmpSink(void);

        uint32_t write(Sample& sample);
    private:
        std::string rtmp_url_;
        RTMP rtmp_;
        uint32_t last_error_;
    };
}
