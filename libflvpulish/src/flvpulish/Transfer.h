#pragma once
#include "MuxerBase.h"

namespace record
{
    class Transfer
    {
    public:
        Transfer(void);
        virtual ~Transfer(void);
        virtual void transfer(Sample & sample) = 0;
        void check_transfer(Sample & sample);

    };
}
