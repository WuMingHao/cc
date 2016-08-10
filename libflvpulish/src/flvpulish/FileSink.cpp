#include "FileSink.h"

namespace record
{
    FileSink::FileSink(const char* url)
        : file_(NULL)
    {
        //open file
        file_ = fopen(url,"wb");
    }

    FileSink::~FileSink(void)
    {
        if(file_)
        {
            fclose(file_);
            file_ = NULL;
        }
    }
    uint32_t FileSink::write(Sample& sample)
    {
        if(NULL == file_)
        {
            return 0;
        }

        std::deque<const_buffer>::iterator iter = sample.data.begin();

        size_t iWrite = 0;
        for(; iter != sample.data.end(); ++iter)
        {
            iWrite += fwrite( (*iter).data(),1,(*iter).length(),file_ );
        }

        return iWrite;
    }
}
