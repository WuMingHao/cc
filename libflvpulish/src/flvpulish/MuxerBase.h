#pragma once
#include "common.h"
#include <vector>

namespace record
{
    class Transfer;
    class Sink;

    struct VideoInfo
    {
        uint32_t width;
        uint32_t height;
        uint32_t frame_rate;
        uint32_t bitrate;
    };

    struct AudioInfo
    {
        uint32_t channel_count;
        uint32_t sample_size;
        uint32_t sample_rate;
        uint32_t bitrate;
    };

    struct MediaInfo
    {
        enum StreamType
        {
            AUDIO = 0x08,
            VIDEO = 0x09,
            METADATA = 0x12,
            OTHERS
        };

        MediaInfo()
            : time_scale(0)
            , type(OTHERS)
        {
        }

        ~MediaInfo()
        {
            clear();
        }

        void clear()
        {
            format_data.clear();
            transfers.clear();
        }

        StreamType type;

        union {
            VideoInfo video_format;
            AudioInfo audio_format;
        };
        uint32_t time_scale;
        std::vector<uint8_t> format_data; //±àÂë²ÎÊý
        std::vector<Transfer *> transfers;
    };

    struct Sample
    {
        Sample()
            : flags(0)
            , time(0)
            , cts_delta(0)
            , time_scale(0)
            , size(0)
            , context(NULL)
            , media_info(NULL)
        {
        }

        ~Sample()
        {
            data.clear();
        }

        enum FlagEnum
        {
            sync = 1, 
            discontinuity = 2,
            stream_changed = 4,
        };

        uint32_t itrack;
        uint32_t flags;
        uint32_t time;
        uint32_t cts_delta;
        uint32_t time_scale;
        uint32_t size;
        void * context;
        MediaInfo*  media_info;
        std::deque<const_buffer> data;
    };

    class MuxerBase
    {
    public:
        MuxerBase();
        virtual ~MuxerBase();

        static MuxerBase* create(const char* type);

        virtual bool open(const char *url) = 0;

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

        uint32_t set_frame(
            uint32_t itrack, 
            Sample& sample);

        uint32_t video_index()
        {
            return video_index_;
        }

        uint32_t audio_index()
        {
            return audio_index_;
        }

        virtual void close() = 0;

        bool is_open();

    protected:
        virtual void fill_header(std::vector<Sample>& samples) = 0;

    protected:
        Sink* sink_;

        bool first_packet_;

        uint32_t video_index_;
        uint32_t audio_index_;

        std::vector<MediaInfo> stream_infos_; 
    };
}
