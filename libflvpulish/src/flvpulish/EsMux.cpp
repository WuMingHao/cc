#include "EsMux.h"
#include "Sink.h"
#include "RmAdtsAudioTransfer.h"
#include "EsVideoTransfer.h"
#include "StreamPakcetTransfer.h"
#include "TimeStampTransfer.h"
#include "EsAudioTransfer.h"
#include "SppsTransfer.h"
#include "flv_common.h"

namespace record
{

    EsMux::EsMux(void)
    {
    }

    EsMux::~EsMux(void)
    {
        
    }

    bool EsMux::open(const char* url)
    {
        sink_ = Sink::create(url,"es");
        return (NULL != sink_);
    }

    uint32_t EsMux::set_audio_info(
        uint32_t sample_rate,
        uint32_t channel_count,
        uint32_t sample_size,
        uint32_t time_scale,
        char * const spec_buffer,
        uint32_t buffer_size)
    {
        MuxerBase::set_audio_info(sample_rate,channel_count,sample_size,time_scale,spec_buffer,buffer_size);
        stream_infos_[audio_index_].transfers.push_back(new TimeStampTransfer());
        stream_infos_[audio_index_].transfers.push_back(new RmAdtsAudioTransfer());
        stream_infos_[audio_index_].transfers.push_back(new EsAudioTransfer());
        return 0;
    }


    uint32_t EsMux::set_video_info(
        uint32_t width,
        uint32_t height,
        uint32_t frame_rate,
        uint32_t time_scale,
        char * const spec_buffer,
        uint32_t buffer_size)
    {
        MuxerBase::set_video_info(width,height,frame_rate,time_scale,spec_buffer,buffer_size);
        stream_infos_[video_index_].transfers.push_back(new SppsTransfer());
        stream_infos_[video_index_].transfers.push_back(new TimeStampTransfer());
        stream_infos_[video_index_].transfers.push_back(new StreamPakcetTransfer());
        stream_infos_[video_index_].transfers.push_back(new EsVideoTransfer());
        return 0;
    }

    void EsMux::fill_header(std::vector<Sample>& samples)
    {
        samples.resize(3);

         //WriteMetaData(samples[0]);
        
        static uint8_t nale_header[4] = {0x00,0x00,0x00,0x01};
        

        if(video_index_ != -1)
        {
            Sample& sample = samples[1];

            H264TagHeader header;
            header.AVCPacketType = 0;
            header.OArchive();
            sample.data.push_back(const_buffer((uint8_t *)&header,sizeof(header),true));
            sample.size += sizeof(header);

            MediaInfo &info = stream_infos_[video_index_];
            sample.media_info = &info;
            sample.data.push_back(const_buffer((uint8_t *)&info.format_data[0],info.format_data.size()));
            sample.size += info.format_data.size();
        }

        if(audio_index_ != -1)
        {
            Sample& sample = samples[2];

            AacTagHeader header;
            header.AACPacketType = 0;
            sample.data.push_front(const_buffer((uint8_t*)&header,sizeof(header),true));
            sample.size += sizeof(header);

            MediaInfo &info = stream_infos_[audio_index_];
            sample.media_info = &info;
            sample.data.push_back(const_buffer((uint8_t *)&info.format_data[0],info.format_data.size()));
            sample.size += info.format_data.size();
        }
    }


    void EsMux::WriteMetaData(Sample& sample)
    {
        MetaData medate;
        uint8_t buf[2048] = "";
        uint8_t *p = buf;


        if (audio_index_ != -1)
        {
            medate.has_audio = true;
            medate.sample_rate = stream_infos_[audio_index_].audio_format.sample_rate;

        }

        if (video_index_ != -1)
        {
            medate.has_video = true;
            medate.width = stream_infos_[video_index_].video_format.width;
            medate.height = stream_infos_[video_index_].video_format.height;
        }

        medate.init_data();
        medate.write(p);

        uint32_t size = p-buf;

        sample.data.push_back(const_buffer(buf,size,true));
        sample.size += size;
    }

 
}
