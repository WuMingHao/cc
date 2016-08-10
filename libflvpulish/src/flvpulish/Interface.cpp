// Interface.cpp

#define RECORD_SOURCE
#include "Interface.h"

#include "MuxerBase.h"

#include <map>
#include <string>

static uint8_t nalu_header[4] = {0x00,0x00,0x00,0x01};

#ifdef Z_TEST
const static uint8_t t_open = 1;
const static uint8_t t_set_v = 2;
const static uint8_t t_set_a = 3;
const static uint8_t t_set_f = 4;
const static uint8_t t_set_c = 5;

#define WRITE_BUFFER(p,len) if (m_file)\
{\
    if (len) \
        fwrite(p,1,len,m_file);\
}

#define WRITE_FILE(p) WRITE_BUFFER(&p,sizeof(p));

#endif
namespace record
{
    class Interface
    {
    public:
        Interface()
#ifdef Z_TEST
            : m_file(NULL)
#endif
        {
        }

        ~Interface()
        {

        }

        void* open(const char *url)
        {
#ifdef Z_TEST
            if (m_file)
            {
                fclose(m_file);
            }
            m_file = fopen("d://capdata.bin","wb");

            WRITE_FILE(t_open);
            uint32_t length = strlen(url);
            WRITE_FILE(length);
            WRITE_BUFFER(url,length);
            return m_file;
#endif
            std::string tmp_url = url;
            std::string newUrl = url;
            std::string key;

            if (std::string::npos != tmp_url.find("|")) 
            {
                key = tmp_url.substr(0,tmp_url.find("|"));
                newUrl = tmp_url.substr(tmp_url.find("|")+1,tmp_url.size());
            }
            MuxerBase* p = MuxerBase::create(key.c_str());

            if(p->open(newUrl.c_str()))
            {
                //写队列
            }
            else
            {
                delete p; 
                p = NULL;
            }
            return p;
        }

        void close(void* handle)
        {
#ifdef Z_TEST
            WRITE_FILE(t_set_c);
            if (m_file)
            {
                fclose(m_file);
                m_file = NULL;
            }
            return;
#endif
            MuxerBase* mux = (MuxerBase*)handle;
            mux->close();
            delete mux;
        }

        V_uint32 set_audio_info(
            RecordHandle handle,
            V_uint32 sample_rate,
            V_uint32 channel_count,
            V_uint32 sample_size,
            V_uint32 time_scale,
            V_char * const spec_buffer,
            V_uint32 buffer_size)
        {
#ifdef Z_TEST
            WRITE_FILE(t_set_a);
            WRITE_FILE(sample_rate);
            WRITE_FILE(channel_count);
            WRITE_FILE(sample_size);
            WRITE_FILE(time_scale);
            WRITE_FILE(buffer_size);
            WRITE_BUFFER(spec_buffer,buffer_size);
            return 0;
#endif
            MuxerBase* mux = (MuxerBase*)handle;
            return  mux->set_audio_info(sample_rate,channel_count,sample_size,time_scale,spec_buffer,buffer_size);
        }


        V_uint32 set_video_info(
            RecordHandle handle,
            V_uint32 width,
            V_uint32 height,
            V_uint32 frame_rate,
            V_uint32 time_scale,
            V_char * const spec_buffer,
            V_uint32 buffer_size)
        {
#ifdef Z_TEST
            WRITE_FILE(t_set_v);
            WRITE_FILE(width);
            WRITE_FILE(height);
            WRITE_FILE(frame_rate);
            WRITE_FILE(time_scale);
            WRITE_FILE(buffer_size);
            WRITE_BUFFER(spec_buffer,buffer_size);
            return 0;
#endif
            MuxerBase* mux = (MuxerBase*)handle;
            return  mux->set_video_info(width,height,frame_rate,time_scale,spec_buffer,buffer_size);
        }


        

        void parseh264(uint8_t* p, size_t s, std::vector<Sample>& vlist)
        {
            
            uint8_t* beg = NULL;
            uint8_t* end = NULL;

            for(size_t i = 0; s > 3 && i < s-3; ++i)
            {
                if(0 == memcmp(p+i,nalu_header,4))
                {
                    if(NULL == beg)
                    {
                        beg = p+i;
                    }
                    else if(NULL == end)
                    {
                        end = p+i;
                        Sample sample;
                        sample.size = end - beg;
                        sample.data.push_back(const_buffer(beg,sample.size));
                        vlist.push_back(sample);

                        beg = p+i;
                        end =NULL;
                    }
                    i += 3;
                }
                else if(i == 0)
                {
                    beg = p;
                }
            }
            if(s < 4)
            {
                beg = p;
            }
            if(beg != NULL)
            {
                Sample sample;
                sample.size = (s - (beg - p));
                sample.data.push_back(const_buffer(beg,sample.size));
                vlist.push_back(sample);
            }
        }

         void set_config(MuxerBase* mux,std::vector<Sample>& samples)
         {//视频编码参数不全时，先从第一帧数据中找出sps pps
             
             NaluHeader nalu;
             for(size_t i = 0; i < samples.size(); ++i)
             {
                 Sample& sample = samples[i];
                 const_buffer& packet = *sample.data.begin();

                 if(0 == memcmp(packet.data(),nalu_header,4))
                     nalu.set(packet.data()[4]);
                 else
                     nalu.set(packet.data()[0]);
                 if(nalu.nal_unit_type == NaluHeader::SPS 
                     || nalu.nal_unit_type == NaluHeader::PPS)
                 {
                     sample.itrack = mux->video_index();
                     sample.time = 0;
                     sample.cts_delta = 0;
                     mux->set_frame(sample.itrack,sample);
                 }
             }
         }


        V_uint32 set_frame(
            RecordHandle handle,
            RECORD_PUSH_TYPE type, 
            RECORD_Frame const * frame)
        {
#ifdef Z_TEST
            WRITE_FILE(t_set_f);
            WRITE_FILE(type);
            WRITE_FILE(frame->start_time);
            WRITE_FILE(frame->cts_delta);
            WRITE_FILE(frame->buffer_length);
            WRITE_BUFFER(frame->buffer,frame->buffer_length);
            return 0;
#endif
            MuxerBase* mux = (MuxerBase*)handle;
            std::vector<Sample> samples;
            uint32_t ret = 0;
            //test code
            //uint8_t spspps[] = {0x00,0x00,0x00,0x01,0x67,0xe1,0x00,0x00,0x00,0x01,0x68,0x35,0x00,0x00,0x00,0x01,0x65};
            //parseh264(spspps,sizeof(spspps),samples);

  			if (type == rpt_video)
			{
            parseh264(frame->buffer,frame->buffer_length,samples);
			}
			else
			{
				Sample sample;
				sample.size = frame->buffer_length;
				sample.time = frame->start_time;
				sample.cts_delta = frame->cts_delta;
				sample.data.push_back(const_buffer(frame->buffer,sample.size));
				samples.push_back(sample);
			}

            if (!mux->is_open() && type==rpt_video)
            {
                set_config(mux,samples);
            }

            for(size_t i = 0; i < samples.size(); ++i)
            {
                 Sample& sample = samples[i];
                 sample.itrack = (type==rpt_video)?mux->video_index():mux->audio_index();
                 sample.time = frame->start_time;
                 sample.cts_delta = frame->cts_delta;
                 ret = mux->set_frame(sample.itrack,sample);
                 if(ret != 0)
                    break;
            }
            return ret;
        }

    private:
        std::map<MuxerBase*,std::string> record_list_;

#ifdef Z_TEST
        FILE* m_file;
#endif
    };
}

record::Interface & get_interface()
{
    static record::Interface g_interface;
    return g_interface;
}


#if __cplusplus
extern "C" {
#endif // __cplusplus
    RECORD_DECL  RecordHandle RECORD_Open( const V_char* url)
    {
        return get_interface().open(url);
    }

    RECORD_DECL V_uint32 RECORD_SetAudioInfo(
         RecordHandle handle,
        V_uint32 sample_rate,
        V_uint32 channel_count,
        V_uint32 sample_size,
        V_uint32 time_scale,
        V_char * const spec_buffer,
        V_uint32 buffer_size)
    {
        return get_interface().set_audio_info(handle,sample_rate,channel_count,sample_size,time_scale,spec_buffer,buffer_size);
    }

    RECORD_DECL V_uint32 RECORD_SetVideoInfo(
         RecordHandle handle,
        V_uint32 width,
        V_uint32 height,
        V_uint32 frame_rate,
        V_uint32 time_scale,
        V_char * const spec_buffer,
        V_uint32 buffer_size)
    {
        return get_interface().set_video_info(handle,width,height,frame_rate,time_scale,spec_buffer,buffer_size);
    }

    RECORD_DECL V_uint32 RECORD_RecordPush(
        RecordHandle handle,
        RECORD_PUSH_TYPE type, 
        RECORD_Frame const * frame)
    {
        return get_interface().set_frame(handle,type,frame);
    }

    RECORD_DECL void RECORD_RecordDestory( RecordHandle handle)
    {
        get_interface().close(handle);
    }

#if __cplusplus
}
#endif // __cplusplus
