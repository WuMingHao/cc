#include "Transfer.h"

namespace record
{
    Transfer::Transfer(void)
    {
    }

    Transfer::~Transfer(void)
    {
    }

    void Transfer::check_transfer(Sample & sample)
    {
        if(sample.size)
            transfer(sample);
    }
}
