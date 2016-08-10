#include "EsVideoTransfer.h"

namespace record
{
    EsVideoTransfer::EsVideoTransfer(void)
    {

    }

    EsVideoTransfer::~EsVideoTransfer(void)
    {

    }

    void EsVideoTransfer::transfer(Sample & sample)
    {
        H264TagHeader header;
        if (sample.flags == Sample::sync)
            header.FrameType = 1;
        else
            header.FrameType = 2;
        header.CompositionTime = sample.cts_delta; 
        header.OArchive();
        sample.data.push_front(const_buffer((uint8_t *)&header,sizeof(header),true));
        sample.size += sizeof(header);
    }
}
