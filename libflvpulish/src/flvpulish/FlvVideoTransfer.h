#pragma once
#include "Transfer.h"

namespace record
{
class FlvVideoTransfer :
    public Transfer
{
public:
    FlvVideoTransfer(void);
    ~FlvVideoTransfer(void);
    virtual void transfer(Sample & sample);
};
}
