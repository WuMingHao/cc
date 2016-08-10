#pragma once

#include "common.h"
#include <string>

namespace record
{
#define AMF_BUFFER_FREE(p) if (NULL != p)\
{\
    delete p;\
    p = NULL; \
}
    struct AMFDataType
    {
        enum Enum {
            NUMBER      = 0x00,
            BOOL        = 0x01,
            STRING      = 0x02,
            OBJECT      = 0x03,
            MOVIECLIP   = 0x04,
            _NULL       = 0x05,
            UNDEFINED   = 0x06,
            REFERENCE   = 0x07,
            MIXEDARRAY  = 0x08,
            OBJECT_END  = 0x09,
            ARRAY       = 0x0a,
            DATE        = 0x0b,
            LONG_STRING = 0x0c,
            UNSUPPORTED = 0x0d,
        };
    };

    class BaseAMF
    {
    public:
        BaseAMF(AMFDataType::Enum type)
            : type_(type)
        {

        }

        virtual ~BaseAMF()
        {
            
        }
        //Ð´type
        virtual void write(uint8_t* &p)
        {

        }

        //²»Ð´ type
        virtual void write1(uint8_t* &p)
        {

        }
        template<class T>
        void write(uint8_t* &p, T t)
        {
            local_to_bigendian((uint8_t *)&t,sizeof(t));
            memcpy(p,&t,sizeof(t));
            p += sizeof(t);
        }

        void write(uint8_t* &p, uint8_t* t, uint32_t size)
        {
            memcpy(p,t,size);
            p += size;
        }

        void write_type(uint8_t* &p)
        {
            uint8_t type =  (uint8_t)type_;
            write(p,type);
        }

    private:
        AMFDataType::Enum type_;
    };

    class ECMAString : public BaseAMF
    {
    public:
        ECMAString()
            : BaseAMF(AMFDataType::STRING)
        {

        }

        ECMAString(const std::string &key)
            : BaseAMF(AMFDataType::STRING)
            , key_(key)
        {

        }

        ECMAString& operator = (const std::string &key)
        {
            key_ = key;
            return *this;
        }

        virtual void write(uint8_t* &p) 
        {
            write_type(p);
            write1(p);
        }

        virtual void write1(uint8_t* &p)
        {
            uint16_t len = key_.size();
            BaseAMF::write(p,len);
            BaseAMF::write(p,(uint8_t*)key_.c_str(),len);
        }
    private:
        std::string key_;
    };


    class ECMABool : public BaseAMF
    {
    public:
        ECMABool()
            : BaseAMF(AMFDataType::BOOL)
            , b_(0)
        {
            
        }

        ECMABool(const bool &key)
            : BaseAMF(AMFDataType::BOOL)
        {
            b_ = key?1:0;
        }

        ECMABool& operator = (const bool &key)
        {
            b_ = key?1:0;
            return *this;
        }

        virtual void write(uint8_t* &p) 
        {
            write_type(p);
            write1(p);
        }

        virtual void write1(uint8_t* &p)
        {
            uint8_t len = 0x1;
            BaseAMF::write(p,b_);
        }
    private:
        uint8_t b_;
    };


    class ECMANumber : public BaseAMF
    {
    public:
        ECMANumber()
            : BaseAMF(AMFDataType::NUMBER)
            , b_(0)
        {

        }

        ECMANumber(const uint32_t &key)
            : BaseAMF(AMFDataType::NUMBER)
        {
            b_ = key;
        }

        ECMANumber& operator = (const uint32_t &key)
        {
            b_ = key;
            return *this;
        }

        virtual void write(uint8_t* &p) 
        {
            write_type(p);
            write1(p);
        }

        virtual void write1(uint8_t* &p)
        {
            //uint8_t len = 0x1;
            BaseAMF::write(p,b_);
        }
    private:
        double b_;
    };

    class EMCGroup
    {
    public:
        EMCGroup
            (BaseAMF* key = NULL,
            BaseAMF* value = NULL)
            : key_(key)
            , value_(value)
        {

        }

        virtual ~EMCGroup()
        {
            AMF_BUFFER_FREE(key_);
            AMF_BUFFER_FREE(value_);
        }

        virtual void write(uint8_t* &p)
        {
            key_->write1(p);
            value_->write(p);
        }
    private:
        BaseAMF* key_;
        BaseAMF* value_;
    };

    class ECMAArray : public BaseAMF
    {
    public:
        ECMAArray()
            : BaseAMF(AMFDataType::MIXEDARRAY)
        {

        }

        virtual ~ECMAArray()
        {
            for(size_t i = 0; i < list_.size(); i++)
            {
                delete  list_[i];
            }
            list_.clear();
        }

        virtual void write(uint8_t* &p)
        {
            BaseAMF::write_type(p);
            uint32_t len = list_.size();
            BaseAMF::write(p,len);
            
            for(size_t i = 0; i < list_.size(); i++)
            {
                list_[i]->write(p);
            }

            uint24_t end = 0x09;
            BaseAMF::write(p,end);
        }

        void push_back(EMCGroup* group)
        {
            list_.push_back(group);
        }

    private:
        std::vector<EMCGroup*> list_;
    };

    struct MetaData
    {
        MetaData()
            : width(0)
            , height(0)
            , sample_rate(0)
            , has_video(false)
            , has_audio(false)
            , filesize(0)
        {
        }
        ECMAString key;
        ECMAArray   list_;

        //video
        uint32_t width;
        uint32_t height;

        //audio
        uint32_t sample_rate;

        bool has_video;
        bool has_audio;

        uint32_t filesize;




        void write(uint8_t* &p)
        {
            key.write(p);
            list_.write(p);
        }

        void init_data()
        {
            key = "onMetaData";

            EMCGroup* params =NULL;
            params = new EMCGroup(new ECMAString("metadatacreator"),new ECMAString("modified"));
            list_.push_back(params);
            params = new EMCGroup(new ECMAString("hasVideo"),new ECMABool(has_video));
            list_.push_back(params);
            params = new EMCGroup(new ECMAString("hasAudio"),new ECMABool(has_audio));
            list_.push_back(params);

            params = new EMCGroup(new ECMAString("width"),new ECMANumber(width));
            list_.push_back(params);
            params = new EMCGroup(new ECMAString("height"),new ECMANumber(height));
            list_.push_back(params);

            params = new EMCGroup(new ECMAString("audiosamplerate"),new ECMANumber(sample_rate));
            list_.push_back(params);

            //params = new EMCGroup(new ECMAString("filesize"),new ECMANumber(filesize));
            //list_.push_back(params);

        }
    };

}