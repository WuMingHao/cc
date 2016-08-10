// Interface.h

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#ifdef WIN32
#ifdef RECORD_SOURCE
#  define RECORD_DECL __declspec(dllexport)
#else
#  define RECORD_DECL __declspec(dllimport)
#endif
#else
#  define RECORD_DECL __attribute__ ((visibility("default")))
#endif

typedef char V_char;
typedef bool V_bool;
typedef unsigned char V_uchar;
typedef int V_int32;
typedef unsigned short V_uint16;
typedef unsigned int V_uint32;
typedef unsigned long long V_uint64;

#if __cplusplus
extern "C" {
#endif // __cplusplus
    enum RECORD_ErrorEnum
    {
        record_success = 0,
        record_openned_before,
        record_others = 1024,
    };

    typedef struct tag_RECORD_frame // IN
    {
        V_uint32 buffer_length;    // Frame的大小
        V_uchar* buffer;    // Frame的内容
        
        V_uint32 start_time;
        V_uint32 cts_delta;
    } RECORD_Frame;

    typedef void* RecordHandle;

    //url:rtmp的url
    RECORD_DECL RecordHandle RECORD_Open( const V_char * url);

    RECORD_DECL V_uint32 RECORD_SetAudioInfo(
        RecordHandle handle,
        V_uint32 sample_rate, //采样率 
        V_uint32 channel_count,  //频道个数
        V_uint32 sample_size,
        V_uint32 time_scale,  // 填1000
        V_char * const spec_buffer, //音频编码参数 aac conf
        V_uint32 buffer_size);

    RECORD_DECL V_uint32 RECORD_SetVideoInfo(
        RecordHandle handle,
        V_uint32 width,
        V_uint32 height,
        V_uint32 frame_rate,
        V_uint32 time_scale, //填1000  
        V_char * const spec_buffer,  //视频编码参数 avc conf
        V_uint32 buffer_size);  

    enum RECORD_PUSH_TYPE
    {
        rpt_video = 0,
        rpt_audio,
    };

    //推流
    RECORD_DECL V_uint32 RECORD_RecordPush(
        RecordHandle handle,
        RECORD_PUSH_TYPE type,
        RECORD_Frame const * frame);

    RECORD_DECL void RECORD_RecordDestory(
        RecordHandle handle);

#if __cplusplus
}
#endif // __cplusplus

#endif // 
