#include "common.h"

void tranverse(uint8_t* data,uint8_t len)
{
    if (len < 2)
    {
        return;
    }

    uint8_t buf;
    for (uint8_t i = 0; i <  len/2; ++i)
    {
        buf = data[i];
        data[i] = data[len-i-1];
        data[len-i-1] = buf;
    }
}

void local_to_bigendian(uint8_t* data,uint8_t len)
{
#ifdef ENABLE_BIG_ENDIAN
#else
    tranverse(data,len);
#endif
}

void bigendian_to_local(uint8_t* data,uint8_t len)
{
#ifdef ENABLE_BIG_ENDIAN
#else
    tranverse(data,len);
#endif
}

