#include "TimeStampTransfer.h"

namespace record
{
    TimeStampTransfer::TimeStampTransfer(void)
    {
    }

    TimeStampTransfer::~TimeStampTransfer(void)
    {
    }

    void TimeStampTransfer::transfer(Sample & sample)
    {
        sample.time = ((uint64_t)sample.time * 1000) / sample.media_info->time_scale;
        sample.cts_delta = ((uint64_t)sample.cts_delta * 1000) / sample.media_info->time_scale;
    }
}
