#include "MuxerBase.h"
#include "Transfer.h"
#include "Sink.h"

#include "EsMux.h"
#include "FlvMux.h"
namespace record
{
    //flv 
     MuxerBase* MuxerBase::create(const char* type)
     {
        if (0 == strcmp("es",type))
        {
            return new EsMux();
        }
        else if (0 == strcmp("flv",type))
        {
            return new FlvMux();
        }
        else
        {
			return new FlvMux();
            //return new EsMux();
        }
     }

    MuxerBase::MuxerBase(void)
        : sink_(NULL)
        , first_packet_(true)
        , video_index_(-1)
        , audio_index_(-1)
    {
    }

    MuxerBase::~MuxerBase(void)
    {
        if(sink_)
        {
            delete sink_;
            sink_ = NULL;
        }

        for (size_t i = 0; i < stream_infos_.size(); ++i)
        {
            MediaInfo& info = stream_infos_[i];
            for (size_t j = 0; j < info.transfers.size(); ++j)
            {
                delete info.transfers[j];
            }
        }
        stream_infos_.clear();
    }

    uint32_t MuxerBase::set_audio_info(
        uint32_t sample_rate,
        uint32_t channel_count,
        uint32_t sample_size,
        uint32_t time_scale,
        char * const spec_buffer,
        uint32_t buffer_size)
    {
        MediaInfo info;
        info.type =  MediaInfo::AUDIO;
        if (buffer_size < 1)
        {
            AacDecoderConfigurationRecord aacConf;
            aacConf.channelConfig = channel_count;
            aacConf.set_FrequencyIndex(aacConf.get_sampleFrequencyIndex(sample_rate));
            aacConf.audioObjectType = AacDecoderConfigurationRecord::aac_lc;
            info.format_data.resize(sizeof(AacDecoderConfigurationRecord));
            memcpy(&info.format_data.at(0), &aacConf, sizeof(AacDecoderConfigurationRecord));
        }
        else
        {
            info.format_data.resize(buffer_size);
            memcpy(&info.format_data.at(0), spec_buffer, buffer_size);
        }

        info.audio_format.bitrate = 0;
        info.audio_format.channel_count = channel_count;
        info.audio_format.sample_rate = sample_rate;
        info.audio_format.sample_size = sample_size;
        info.time_scale = time_scale;

        stream_infos_.push_back(info);
        audio_index_ = stream_infos_.size() - 1;
        return 0;
    }


    uint32_t MuxerBase::set_video_info(
        uint32_t width,
        uint32_t height,
        uint32_t frame_rate,
        uint32_t time_scale,
        char * const spec_buffer,
        uint32_t buffer_size)
    {
        MediaInfo info;
        info.type =  MediaInfo::VIDEO;
        if (buffer_size > 0)
        {
            info.format_data.resize(buffer_size);
            memcpy(&info.format_data.at(0), spec_buffer, buffer_size);
        }

        info.video_format.bitrate = 0;
        info.video_format.frame_rate = frame_rate;
        info.video_format.height = height;
        info.video_format.width = width;
        info.time_scale = time_scale;

        stream_infos_.push_back(info);
        video_index_ = stream_infos_.size() - 1;
        return 0;
    }

    bool MuxerBase::is_open()
    {
       if(stream_infos_.size() < 1)
           return false;
       for (size_t i = 0; i < stream_infos_.size(); ++i)
       {
           MediaInfo& info = stream_infos_[i];
           if(info.format_data.size() < 1)
               return false;
       }
       return true;
    }

    uint32_t MuxerBase::set_frame(
        uint32_t itrack, 
        Sample& sample)
    {
        uint32_t ret = 0;
        if(itrack == -1) return -1;

        sample.media_info = &stream_infos_[itrack];

        for(size_t i = 0; i < sample.media_info->transfers.size() ; ++i)
            sample.media_info->transfers[i]->check_transfer(sample);
        if (sample.size < 1)
        {
            return 0;
        }

        if(first_packet_)
        {
            //return 2 wait for sps pps
            if(!is_open()) return 0;
            first_packet_ = false;

            std::vector<Sample> samples;
            fill_header(samples);
            for (size_t i = 0; i < samples.size(); ++i)
            {
                Sample& sample = samples[i]; 
                if (sample.size )
                {
                   sink_->write(sample);
                }
            }
            
        }
                       
        if (sink_->write(sample))
            ret = 0;
        else 
            ret = -1;

        return ret;
    }
}
