#pragma once
#include "Transfer.h"

namespace record
{
    class TimeStampTransfer : public Transfer
    {
    public:
        TimeStampTransfer(void);
        ~TimeStampTransfer(void);

        virtual void transfer(Sample & sample);
    };
}
