#include "Sink.h"
#include "FlvRtmpSink.h"
#include "EsRtmpSink.h"
#include "FileSink.h"

#define RTMP_URL_HEAD "rtmp://"
#define FILE_URL_HEAD "file://"

#define RTMP_SEND_TYPE_ES "es"
#define RTMP_SEND_TYPE_FLV "flv"

namespace record
{
    //es:file://
    //es:rtmp://
    //flv:file://
    //flv:rtmp://

    Sink* Sink::create(const char* url, const char* type)
    {
        Sink* pSink = NULL;
        if (0 == strncmp(url,RTMP_URL_HEAD,strlen(RTMP_URL_HEAD)))
        {
            if(0 == strncmp(type,RTMP_SEND_TYPE_ES,strlen(RTMP_SEND_TYPE_ES)))
                 pSink =  new EsRtmpSink(url);
            else
                pSink =  new FlvRtmpSink(url);
        }
        else if (0 == strncmp(url,FILE_URL_HEAD,strlen(FILE_URL_HEAD)))
        {
            std::string file_name = url;
            pSink =  new FileSink(file_name.c_str()+strlen(FILE_URL_HEAD));
        }
        return pSink;
    }

    Sink::Sink(void)
    {
        bSuccess = false;
    }

    Sink::~Sink(void)
    {
    }
}
