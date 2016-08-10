#include "flv_common.h"
#include "FlvMux.h"
#include "Sink.h"
#include "StreamPakcetTransfer.h"
#include "FlvVideoTransfer.h"
#include "FlvAudioTransfer.h"
#include "RmAdtsAudioTransfer.h"
#include "TimeStampTransfer.h"
#include "SppsTransfer.h"

namespace record
{

    FlvMux::FlvMux(void)
    {
    }

    FlvMux::~FlvMux(void)
    {

    }

    bool FlvMux::open(const char* url)
    {
        sink_ = Sink::create(url,"flv");
        if(NULL != sink_)
        {
             return sink_->bSuccess;
        }else 
        return false;
    }

    uint32_t FlvMux::set_audio_info(
        uint32_t sample_rate,
        uint32_t channel_count,
        uint32_t sample_size,
        uint32_t time_scale,
        char * const spec_buffer,
        uint32_t buffer_size)
    {
        MuxerBase::set_audio_info(sample_rate,channel_count,sample_size,time_scale,spec_buffer,buffer_size);
        stream_infos_[audio_index_].transfers.push_back(new RmAdtsAudioTransfer());
        stream_infos_[audio_index_].transfers.push_back(new TimeStampTransfer());
        stream_infos_[audio_index_].transfers.push_back(new FlvAudioTransfer());

        return 0;
    }


    uint32_t FlvMux::set_video_info(
        uint32_t width,
        uint32_t height,
        uint32_t frame_rate,
        uint32_t time_scale,
        char * const spec_buffer,
        uint32_t buffer_size)
    {
        MuxerBase::set_video_info(width,height,frame_rate,time_scale,spec_buffer,buffer_size);
        stream_infos_[video_index_].transfers.push_back(new SppsTransfer());
        stream_infos_[video_index_].transfers.push_back(new StreamPakcetTransfer());
        stream_infos_[video_index_].transfers.push_back(new TimeStampTransfer());
        stream_infos_[video_index_].transfers.push_back(new FlvVideoTransfer());
        return 0;
    }

    

    void FlvMux::fill_header(std::vector<Sample>& samples)
    {
        samples.resize(1);
        Sample& sample = samples[0];
        first_packet_ = false;

        FlvHeader flvHeader;

        uint32_t a_pre_size = 0;
        uint32_t v_pre_size = 0;

        flvHeader.TypeFlagsVideo = (video_index_==-1)?0:1;
        flvHeader.TypeFlagsAudio = (audio_index_==-1)?0:1;
        flvHeader.OArchive();

        sample.data.push_back(const_buffer((uint8_t *)&flvHeader,sizeof(flvHeader),true));
        sample.size += sizeof(FlvHeader);
        //WriteMetaData(sample);
        //video
        if(video_index_ != -1)
        {
            MediaInfo& info = stream_infos_[video_index_];
            VideoTag vTag;
            vTag.AVCPacketType = 0;
            vTag.date_size = info.format_data.size()+5;
            vTag.OArchive();

            sample.data.push_back(const_buffer((uint8_t *)&vTag,sizeof(vTag),true));
            sample.size += sizeof(vTag);
            sample.data.push_back(const_buffer((uint8_t *)&info.format_data[0],info.format_data.size()));
            sample.size += info.format_data.size();

            v_pre_size = sizeof(vTag) + info.format_data.size();
            local_to_bigendian((uint8_t *)&v_pre_size,sizeof(uint32_t));

            sample.data.push_back(const_buffer((uint8_t *)&v_pre_size,sizeof(uint32_t),true));
            sample.size += sizeof(uint32_t);
        }

        if(audio_index_ != -1)
        {
            MediaInfo& info = stream_infos_[audio_index_];
            AudioTag aTag;
            aTag.AACPacketType = 0;
            aTag.date_size = info.format_data.size()+2;

            uint32_t sample_rate = info.audio_format.sample_rate;
            if (sample_rate >= 44100 ) {
                aTag.SoundRate = 3;
            } else if (sample_rate >= 22000 ){
                aTag.SoundRate = 2;
            } else if (sample_rate >= 11000) {
                aTag.SoundRate = 1;
            } else if (sample_rate >= 5500) {
                aTag.SoundRate = 0;
            } else {
                aTag.SoundRate = 0;
            }
            aTag.OArchive();
            sample.data.push_back(const_buffer((uint8_t *)&aTag,sizeof(aTag),true));
            sample.size += sizeof(aTag);

            sample.data.push_back(const_buffer((uint8_t *)&info.format_data[0],info.format_data.size()));
            sample.size += info.format_data.size();
            
            a_pre_size = sizeof(aTag) + info.format_data.size();
            local_to_bigendian((uint8_t *)&a_pre_size,sizeof(uint32_t)); 

            sample.data.push_back(const_buffer((uint8_t *)&a_pre_size,sizeof(uint32_t),true));
            sample.size += sizeof(uint32_t);
        }
    }


    void FlvMux::WriteMetaData(Sample& sample)
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

        FlvTag tag;
        memset(&tag,0,sizeof(tag));
        tag.Type = 0x12;
        tag.date_size = size;
        tag.OArchive();

        sample.data.push_back(const_buffer((uint8_t *)&tag,sizeof(tag),true));
        sample.size += sizeof(tag);

        sample.data.push_back(const_buffer(buf,size,true));
        sample.size += size;

        uint32_t pre_size = sizeof(tag) + size;
        local_to_bigendian((uint8_t *)&pre_size,sizeof(uint32_t)); 

        sample.data.push_back(const_buffer((uint8_t *)&pre_size,sizeof(uint32_t)));
        sample.size += sizeof(uint32_t);
    }
}
