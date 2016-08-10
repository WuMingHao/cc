#pragma once
#include "MuxerBase.h"

namespace record
{
    class FlvMux : public MuxerBase
    {
    public:
        FlvMux(void);
        virtual ~FlvMux(void);

        bool open(const char* url);

        virtual uint32_t set_audio_info(
            uint32_t sample_rate,
            uint32_t channel_count,
            uint32_t sample_size,
            uint32_t time_scale,
            char * const spec_buffer,
            uint32_t buffer_size);


        virtual  uint32_t set_video_info(
            uint32_t width,
            uint32_t height,
            uint32_t frame_rate,
            uint32_t time_scale,
            char * const spec_buffer,
            uint32_t buffer_size);


        void close()
        {

        }

    private:
        void fill_header(std::vector<Sample>& samples);
        void WriteMetaData(Sample& sample);
    };
}
