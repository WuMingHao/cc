#ifndef __COMMON_H__ 
#define __COMMON_H__

#include <string.h>
#include <deque>
#include <vector>

#pragma pack (1)

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

typedef int int32_t;

struct uint24_t
{
    uint24_t()
    {
        clear();
    }

    uint24_t(uint32_t ii)
    {
        clear();
        memcpy(data,&ii,sizeof(data));
    }

    void clear()
    {
        memset(data,0,sizeof(data));
    }

    operator uint32_t ()
    {
        uint32_t ii = 0;
        memcpy(&ii,data,sizeof(data));
        return ii;
    }

    uint24_t& operator =(const uint32_t& ii)
    {
        memcpy(this->data,&ii,sizeof(uint24_t));
        return *this;
    }

    uint8_t data[3];

};


void local_to_bigendian(uint8_t* data,uint8_t len);
void bigendian_to_local(uint8_t* data,uint8_t len);


struct Object
{
    template<class T>
    void local_bigendian(T& i)
    {
        local_to_bigendian((uint8_t*)&i,sizeof(T)/sizeof(uint8_t));
    }

    template<class T>
    void bigendian_local(T& i)
    {
        bigendian_to_local((uint8_t*)&i,sizeof(T)/sizeof(uint8_t));
    }

    void IArchive() //for read
    {

    }

    void OArchive() // for write
    {
    }
};

struct NaluHeader
{
    enum NaluTypeEnum
    {
        undefine = 0,
        UNIDR,  //p帧
        DATABLOCK_A,
        DATABLOCK_B,
        DATABLOCK_C,
        IDR,       //I帧
        SEI,
        SPS,
        PPS,
        AccessUnitDelimiter,//分界符
        EndOfSeq,
        EndOfStream,
        FillerData,
        SPSExtension,
        Other,
    };

    NaluHeader()
    {

    }

    NaluHeader(
        uint8_t b)
    {
        set(b);
    }

    void set(uint8_t b)
    {
        nal_ref_idc = b & 0x60;
        nal_unit_type = b & 0x1f;
    }

    uint8_t forbidden_zero_bit : 1;
    uint8_t nal_ref_idc : 2;
    uint8_t nal_unit_type : 5;
};

struct SliceHeader
{
    enum FrameEnum
    {
        I = 0,
        B,  //p帧
        P,
    };

    SliceHeader(
        uint8_t b)
    {
        set(b);
    }

    void set(uint8_t b)
    {
        type = b >> 6;
    }

    uint8_t type : 2;
    uint8_t Reserved : 6;
};




struct AVCDecoderConfigurationRecord : public Object 
{
    AVCDecoderConfigurationRecord()
        : configurationVersion(0x01)
        , AVCProfileIndication(0) //sps[1]
        , profile_compatibility(0) //sps[2]
        , AVCLevelIndication(0)  //sps[3]
        , flag1(0xff)
        , flag2(0xe1)
        , sequenceParameterSetLength(0)  //sps len
        , sequenceParameterSetNALUnit(NULL) //sps
        , numOfPictureParameterSets(0x01)
        , pictureParameterSetLength(0) // pps len
        , pictureParameterSetNALUnit(NULL) //pps
    {

    }
    uint8_t configurationVersion ;
    uint8_t AVCProfileIndication;
    uint8_t profile_compatibility;
    uint8_t AVCLevelIndication;

    union {
        struct {
            uint8_t lengthSizeMinusOne : 2; /*这个参数加上1也就是4描述的是在slice里面对于nalu长度描述中的byte的个数（默认是4）*/
            uint8_t reserved : 6;
        };
        uint8_t flag1;
    };

    union {
        struct {
            uint8_t numOfSequenceParameterSets : 5;
            uint8_t reserved1 : 3;
        };
        uint8_t flag2;
    };

    uint16_t sequenceParameterSetLength ;
    uint8_t* sequenceParameterSetNALUnit;
    uint8_t numOfPictureParameterSets;
    uint16_t pictureParameterSetLength ;
    uint8_t* pictureParameterSetNALUnit;

    void set_sps(const uint8_t* sps, uint16_t len)
    {
        if (sequenceParameterSetNALUnit == NULL)
        {
            sequenceParameterSetNALUnit = new uint8_t[len];
            memcpy(sequenceParameterSetNALUnit,sps,len);
            sequenceParameterSetLength = len;

            AVCProfileIndication = sps[1];
            profile_compatibility = sps[2];
            AVCLevelIndication = sps[3];
        }
    }

    bool is_full()
    {
        return (pictureParameterSetLength&&sequenceParameterSetLength);
    }

    void set_pps(const uint8_t* pps, size_t len)
    {
        if (pictureParameterSetNALUnit == NULL)
        {
            pictureParameterSetNALUnit = new uint8_t[len];
            memcpy(pictureParameterSetNALUnit,pps,len);
            pictureParameterSetLength = len;
        }
    }

    void to_data(std::vector<uint8_t>& vec)
    {
        size_t len = 6 + 2 + sequenceParameterSetLength + 1 + 2 + pictureParameterSetLength;
        vec.resize(len);
        uint8_t* p = &vec.at(0);
        memcpy(p, this, 6);
        p += 6;


        uint16_t sps_len = sequenceParameterSetLength;
        local_to_bigendian((uint8_t*)&sps_len,sizeof(uint16_t));
        memcpy(p, &sps_len, sizeof(uint16_t));
        p +=  sizeof(uint16_t);

        if (sequenceParameterSetLength)
        {
            memcpy(p,sequenceParameterSetNALUnit,sequenceParameterSetLength);
            p +=  sequenceParameterSetLength;
        }
        
        memcpy(p, &numOfPictureParameterSets, sizeof(uint8_t));
        p +=  sizeof(uint8_t);


        uint16_t pps_len = pictureParameterSetLength;
        local_to_bigendian((uint8_t*)&pps_len,sizeof(uint16_t));
        memcpy(p, &pps_len, sizeof(uint16_t));
        p +=  sizeof(uint16_t);

        if (pictureParameterSetLength)
        {
            memcpy(p,pictureParameterSetNALUnit,pictureParameterSetLength);
            p +=  pictureParameterSetLength;
        }
    }

};

struct AacDecoderConfigurationRecord : public Object 
{
    AacDecoderConfigurationRecord()
        : audioObjectType(0)
        , sampleFrequencyIndex1(0)
        , sampleFrequencyIndex2(0)
        , channelConfig(0)
        , frameLengthFlag(0)
        , depends(0)
        , extensionFlag(0)
    {

    }

    enum AacProfile
    {
        aac_main = 1,
        aac_lc,
        aac_sbr,
        aac_reserved,
    };

    uint32_t get_sampleFrequencyIndex(uint32_t sample_rate)
    {
        switch (sample_rate)
        {
        case 96000:
            return 0;
        case 88200:
            return 1;
        case 64000:
            return 2;
        case 48000:
            return 3;
        case 44100:
            return 4;
        case 32000:
            return 5;
        case 24000:
            return 6;
        case 22050:
            return 7; 
        case 16000:
            return 8;
        case 12000:
            return 9;
        case 11025:
            return 10;
        case 8000:
            return 11;
        case 7350:
            return 12;
        default:
            break;
        }
        return  4;
    }

    void set_FrequencyIndex(uint8_t index)
    {
        index &= 0x0f;
        sampleFrequencyIndex1 = index>>1;
        sampleFrequencyIndex2 = index&0x01;
    }

    uint8_t sampleFrequencyIndex1 : 3; //4对应的是 44100
    uint8_t audioObjectType : 5; //aac-lc 2


    uint8_t frameLengthFlag : 1;  //0
    uint8_t depends : 1;  //0
    uint8_t extensionFlag : 1; //0 
    uint8_t channelConfig : 4;  //声道数
    uint8_t sampleFrequencyIndex2 : 1; //4对应的是 44100
};


struct FlvHeader : public Object 
{
    FlvHeader()
        :  version(1)
        , flag(0) // TypeFlagsReserved = 0, TypeFlagsAudio = 1, TypeFlagsReserved2 = 0, TypeFlagsVideo = 0
        , DataOffset(9)
        , PreTagSize(0)
    {
        memcpy(type,"FLV",3);
    }

    void OArchive()
    {
        Object::OArchive();
        local_bigendian(DataOffset);
        local_bigendian(PreTagSize);
    }

    uint8_t type[3];
    uint8_t version;
    union {
        struct {
            uint8_t TypeFlagsVideo : 1;
            uint8_t TypeFlagsReserved2 : 1;
            uint8_t TypeFlagsAudio : 1;
            uint8_t TypeFlagsReserved : 5;
        };
        uint8_t flag;
    };
    uint32_t DataOffset;
    uint32_t PreTagSize;
};

struct stream_tag_type
{
    enum Enum {
        AUDIO = 0x08,
        VIDEO = 0x09,
        METADATA = 0x12
    };
};

struct FlvTag : public Object 
{
    FlvTag()
        : flag(0)
        , date_size(0)
        , timestample(0)
        , extent_time(0)
        , streamsID(0)
    {

    }
    void OArchive()
    {
        local_bigendian(date_size);
        local_bigendian(timestample);
        local_bigendian(streamsID);
    }
    /*
    8 -- 音频tag
    9 -- 视频tag
    18 -- 脚本tag
    */
    union {
        struct {
            uint8_t  Type : 5;
            uint8_t  Filter : 1;
            uint8_t  Reserved : 2;
        };
        uint8_t flag;
    };
    uint24_t date_size;
    uint24_t timestample;
    uint8_t extent_time;
    uint24_t streamsID;
};

struct AudioTag : public FlvTag
{
    AudioTag()
        : SoundFormat(10)
        , SoundRate(3)
        , SoundSize(1)
        , SoundType(1)
        , AACPacketType(1)
    {
        FlvTag::Type = stream_tag_type::AUDIO;
    }

    void OArchive()
    {
        FlvTag::OArchive();
        local_bigendian(AACPacketType);
    }
    union {
        struct {
            uint8_t SoundType : 1;
            uint8_t SoundSize : 1;
            uint8_t SoundRate : 2;
            uint8_t SoundFormat : 4;
        };
        uint8_t au_flag;
    };
    uint8_t AACPacketType;
};


struct AacTagHeader  : public Object
{
    AacTagHeader()
        : SoundFormat(10)
        , SoundRate(3)
        , SoundSize(1)
        , SoundType(1)
        , AACPacketType(1)
    {
    }

    union {
        struct {
            uint8_t SoundType : 1;
            uint8_t SoundSize : 1;
            uint8_t SoundRate : 2;
            uint8_t SoundFormat : 4;
        };
        uint8_t au_flag;
    };
    uint8_t AACPacketType;
};

struct H264TagHeader : public Object
{
    H264TagHeader()
        : FrameType(1)
        , CodecID(7)
        , AVCPacketType(1)
        , CompositionTime(0)
    {
    }

    void OArchive()
    {
        local_bigendian(AVCPacketType);
        local_bigendian(CompositionTime);
    }

    uint8_t CodecID : 4;
    uint8_t FrameType : 4;
    uint8_t AVCPacketType;
    uint24_t CompositionTime;
};


struct VideoTag : public FlvTag
{
    VideoTag()
        : FrameType(1)
        , CodecID(7)
        , AVCPacketType(1)
        , CompositionTime(0)
    {
        FlvTag::Type = stream_tag_type::VIDEO;
    }

    void OArchive()
    {
        FlvTag::OArchive();
        local_bigendian(AVCPacketType);
        local_bigendian(CompositionTime);
    }

    uint8_t CodecID : 4;
    uint8_t FrameType : 4;
    uint8_t AVCPacketType;
    uint24_t CompositionTime;
};

#pragma pack ()

class const_buffer
{
public:
    const_buffer()
        : buf_(NULL)
        , data_(NULL)
        , len_(0)
        , free_flag_(false)
    {

    }

    const_buffer(const const_buffer& obj)
    {
        free_flag_ = obj.free_flag_;
        if(free_flag_)
        {
            buf_ = new uint8_t[obj.len_];
            memcpy(buf_,obj.buf_,obj.len_);
           
        }
        else
        {
            buf_ = obj.buf_;
        }
        data_ =  buf_;
        len_ = obj.len_;
    }

    const_buffer(uint8_t* buf,uint32_t len,bool free_flag = false)
        : len_(len)
    {
        if(free_flag_ = free_flag)
        {  //true
            buf_ = new uint8_t[len_];
            memcpy(buf_,buf,len_);
        }
        else
        {
            buf_ = buf;
        }
        data_ = buf_;
    }

    ~const_buffer()
    {
        if(buf_ && free_flag_)
        {
            delete [] buf_;
        }
        buf_ = NULL;
        data_ = NULL;
        free_flag_ = false;
        len_ = 0;
    }

    void operator  +=(const uint32_t &offset)
    {
       if(offset < len_)
       {
            len_ -= offset;
            data_ += offset;
       }
       else if(offset == len_)
       {
           len_ = 0;;
           data_ = NULL;
       }
       else
       {
           len_ = 0;;
           data_ = NULL;
       }
    }

    uint8_t* data()
    {
        return data_;
    }

    uint32_t length()
    {
        return len_;
    }

private:
    uint8_t* buf_;
    uint8_t* data_;
    uint32_t len_;
    bool free_flag_;
};


#endif