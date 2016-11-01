
#ifndef __BARREL_STRING_H__
#define __BARREL_STRING_H__

#include "btype.h"

#define QBSTRING_LONG 0x100000

typedef struct bString_t bString;

struct bString_t
{
    Count  m_len;  //the length of the string
    Count  m_size; //the total size of the buffer, maybe more than length
    Char  *m_buf;  //bString_t is not tight, but it is stable(bString pointer will be always valid)
};

#endif //! __BARREL_STRING_H__