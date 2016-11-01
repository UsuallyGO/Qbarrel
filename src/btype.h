
#ifndef __BARREL_TYPE_H__
#define __BARREL_TYPE_H__

#include <stddef.h>

#define QBARREL_TYPE_STRING 0
#define QBARREL_TYPE_LIST   1
#define QBARREL_TYPE_HASH   2
#define QBARREL_TYPE_SET    3

#define QBARREL_ENCODING_INT        0
#define QBARREL_ENCODING_SKIPLIST   1
#define QBARREL_ENCODING_LINKDLIST  2

typedef int Int;
typedef unsigned int UInt;
typedef long Long;
typedef unsigned long ULong;
typedef double Double;
typedef int long long Int64;
typedef unsigned long long UInt64;
typedef size_t Count;
typedef char Char;
typedef unsigned char UChar;

typedef void* VPtr;
typedef const void* CVPtr;

#endif //! __BARREL_TYPE_H__