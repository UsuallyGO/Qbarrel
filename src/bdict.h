
#ifndef __BARREL_DICT_H__
#define __BARREL_DICT_H__

#include "btype.h"

#define HASH_INITIAL_SIZE 255
#define HASH_INITIAL_MASK 0xff 

#define HASH_GROWABLE 1
#define HASH_FIXED    0

#define HASH_NO_REHASHING -1

#define IsRehashing(x) ((x)->m_reidx != -1)

typedef struct qDict_t qDict;
typedef struct qDictOption_t qDictOption;
typedef struct qDictTable_t  qDictTable;
typedef struct qDictEntry_t  qDictEntry;

struct qDictOption_t
{
    UInt (*hashFunc)(CVPtr key);
    VPtr (*keyDup)  (CVPtr key);
    VPtr (*valDup)  (CVPtr val);
    Int  (*keyCompare)(CVPtr key1, CVPtr key2);
    Int  (*valCompare)(CVPtr val1, CVPtr val2);
    void (*keyDestroy)(VPtr key);
    void (*valDestroy)(VPtr val);
};

struct qDictEntry_t
{
    VPtr m_key;
    union
    {
        VPtr    m_ptr;
        Int64   m_int64;
        UInt64  m_u64;
    }m_value;

    qDictEntry *m_next;
};

struct qDictTable_t
{
    qDictEntry **m_table;
    ULong m_size;
    ULong m_mask;
    ULong m_used;
};

struct qDict_t
{
    qDictOption *m_opt;
    qDictTable  *m_sheets[2];

    Int m_grow;//hash table grows automated
    Int m_reidx;//-1 means no rehash, or the index of rehash
};

#endif //!__BARREL_DICT_H__
