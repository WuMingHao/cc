#pragma once
#include "Transfer.h"

namespace record
{
    class EsVideoTransfer :
        public Transfer
    {
    public:
        EsVideoTransfer(void);
        ~EsVideoTransfer(void);
        virtual void transfer(Sample & sample);
    };
}
