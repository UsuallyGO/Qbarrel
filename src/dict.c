
#include <string.h>
#include <assert.h>
#include "bmemory.h"
#include "bdict.h"
#include "btype.h"

#include "qbarrel.h"

#define HASH_FUNC_SEED 5381

static VPtr _KeyStrDup(CVPtr key)
{
    char* dkey;

    dkey = _bstrdup((const char*)key);
    return dkey;
}

static VPtr _ValStrDup(CVPtr val)
{
    char* dval;//dup value

    dval = _bstrdup((const char*)val);
    return dval;
}

static int _bstrcmp(const char *p1, const char *p2)
{
    return strcmp(p1, p2);
}

static Int _KeyStrCmp(CVPtr k1, CVPtr k2)
{
    return _bstrcmp(k1, k2);
}

static Int _ValStrCmp(CVPtr v1, CVPtr v2)
{
    return _bstrcmp(v1, v2);
}

static void _dupstrdes(char *psz)
{
    if(psz != NULL)
        qfree(psz);
}

static void _KeyStrDes(VPtr key)
{
    _dupstrdes(key);
}

static void _ValStrDes(VPtr val)
{
    _dupstrdes(val);
}

static UInt _GenHashFunc(CVPtr key, int len)
{
    UInt seed = HASH_FUNC_SEED;
    const UInt m = 0x5bd1e995;
    const Int r = 24;
    const UChar *data = (const UChar *)key;
    UInt k, h;

    h = seed ^ len;//Initialize the hash to a 'random' value

    while(len >= 4)
    {
        k = *(UInt*)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data +=4;
        len -= 4;        
    }

    switch(len)
    {
    case 3: h ^= data[2] << 16;
    case 2: h ^= data[1] << 8;
    case 1: h ^= data[0]; h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return (UInt)h;
}

static UInt _HashFunc(CVPtr key)
{
    int len;

    len = strlen(key);
    return _GenHashFunc(key, len);
}

static void _dictableReset(qDictTable **table)
{
    int index;
    qDictTable *tb;

    tb = *(qDictTable**)table;
    tb->m_size = HASH_INITIAL_SIZE;
    tb->m_mask = HASH_INITIAL_MASK;
    tb->m_used = 0;

    tb->m_table = (qDictEntry**)qmalloc(sizeof(qDictEntry*)*HASH_INITIAL_SIZE);
    assert(tb->m_table != NULL);//Perhaps 'assert' should be forbidden here
    memset(tb->m_table, 0, sizeof(qDictEntry*)*HASH_INITIAL_SIZE);
}

static qDictOption _dictStrStrOpt =
{
    _HashFunc,
    _KeyStrDup,
    _ValStrDup,
    _KeyStrCmp,
    _ValStrCmp,
    _KeyStrDes,
    _ValStrDes,
};

QB_PUBLIC qDict* qdict_new()
{
    qDict *dict;

    dict = (qDict*)qmalloc(sizeof(qDict));
    return dict;
}

QB_PUBLIC qDict* qdict_create(int opt, int grow)
{
    qDict *dict;
    (void)opt;//for avoid warnnings

    dict = (qDict*)qmalloc(sizeof(qDict));
    if(dict == NULL)
        return NULL;

    dict->m_opt = &_dictStrStrOpt;
    dict->m_sheets[0] = (qDictTable*)qmalloc(sizeof(qDictTable));
    if(dict->m_sheets[0] == NULL)
    {
        qfree(dict);
        return NULL;
    }
    _dictableReset(&dict->m_sheets[0]);

    dict->m_sheets[1] = NULL;
    dict->m_grow = HASH_GROWABLE;//default is growable
    if(grow == HASH_FIXED)
        dict->m_grow = HASH_FIXED;
    dict->m_reidx = HASH_NO_REHASHING;

    return dict;
}

//Inside function(function name starts with '_') do not check the parameters again
//Check it before you call it
static qDictEntry* _dictFindEntry(qDict *dict, Int index, CVPtr key)
{
    int idx;
    qDictOption *opt;
    qDictTable  *table;
    qDictEntry  *entry;

    opt = dict->m_opt;
    table = dict->m_sheets[index];
    if(table->m_size == 0)
        return NULL;//this table is empty

    idx = opt->hashFunc(key);
    idx = idx & table->m_mask;
    entry = table->m_table[idx];
    while(entry)
    {
        if(opt->keyCompare(entry->m_key, key) == 0)
            return entry;
        entry = entry->m_next;
    }

    return entry;//return null when cannot find
}

//No duplicate key in qDict
static qDictEntry* _dictFindValue(qDict *dict, Int index, CVPtr key, CVPtr value)
{
    qDictOption *opt;
    qDictEntry  *entry;

    entry = _dictFindEntry(dict, index, key);
    if(entry == NULL)
        return NULL;

    opt = dict->m_opt;
    if(opt->valCompare(entry->m_value.m_ptr, value) == 0)
        return entry;
	else
        return NULL;
}

QB_PUBLIC void* qdict_get(qDict *dict, const void *key)
{
    qDictEntry *entry;
    void *value;

    if(dict == NULL || key == NULL)
        return NULL;

    entry = NULL;
    if(IsRehashing(dict))
        entry = _dictFindEntry(dict, 1, key);

    if(entry == NULL)//entry may not rehash to sheet 1, try to find in sheet 0
        entry = _dictFindEntry(dict, 0, key);

    value = NULL;
    if(entry != NULL)
        value = entry->m_value.m_ptr;

    return value;
}

QB_PUBLIC int qdict_find(qDict *dict, const void *key, const void *value)
{
    qDictEntry *entry;

    if(dict == NULL || key == NULL || value == NULL)
        return QBARREL_ERR_INVAL;

    if(IsRehashing(dict))
    {
        entry = _dictFindValue(dict, 1, key, value);
        if(entry != NULL)
            return QBARREL_SUCCESS;
    }

    entry = _dictFindValue(dict, 0, key, value);
    if(entry != NULL)
        return QBARREL_SUCCESS;
    else
        return QBARREL_ERR_NOTFOUND;
}

QB_PUBLIC int qdict_set(qDict *dict, const void *key, const void *value)
{
    int index;
    qDictEntry  *entry;
    qDictOption *opt;
    qDictTable  *table;

    if(dict == NULL || key == NULL || value == NULL)
        return QBARREL_ERR_INVAL;

    entry = NULL;
    opt = dict->m_opt;
    if(IsRehashing(dict))
    {
        entry = _dictFindEntry(dict, 1, key);
    }
    if(entry == NULL)
    {
        entry = _dictFindEntry(dict, 0, key);
    }

    if(entry != NULL)
    {
        if(opt->valCompare(entry->m_value.m_ptr, value) != 0)
        {
            opt->valDestroy(entry->m_value.m_ptr);
            entry->m_value.m_ptr = opt->valDup(value);
        }//else: the same key-value, do nothing
    }
    else//new entry, add into rehash table, if is rehashing now
    {
        entry = (qDictEntry*)qmalloc(sizeof(qDictEntry));
        if(entry == NULL)
            return QBARREL_ERR_NOMEM;

        if(IsRehashing(dict))
            table = dict->m_sheets[1];
        else
            table = dict->m_sheets[0];

        index = opt->hashFunc(key);
        index = index & table->m_mask;
        if(table->m_table[index] == NULL)
            table->m_used++;

        entry->m_key   = opt->keyDup(key);
        entry->m_next  = table->m_table[index];
        entry->m_value.m_ptr = opt->valDup(value);
        table->m_table[index] = entry;
    }

    return QBARREL_SUCCESS;
}

//qdict_add() is different with qdict_set(), in qdict_set(), it will just update the 
//value when the key is already exist in table. But in qdict_add(), we only add 
//a new entry when key is not in the table.
QB_PUBLIC int qdict_add(qDict *dict, const void *key, const void *value)
{
    int index;
    qDictOption *opt;
    qDictTable  *table;
    qDictEntry  *entry;

    if(dict == NULL || key == NULL || value == NULL)
        return QBARREL_ERR_INVAL;

    table = dict->m_sheets[0];
    entry = _dictFindEntry(dict, 0, key);
    if(entry != NULL)
        return QBARREL_ERR_DUP;
    if(IsRehashing(dict))
    {
        entry = _dictFindEntry(dict, 1, key);
        if(entry != NULL)
            return QBARREL_ERR_DUP;
        table = dict->m_sheets[1];
    }

    entry = (qDictEntry*)qmalloc(sizeof(qDictEntry));
    if(entry == NULL)
        return QBARREL_ERR_NOMEM;

    opt   = dict->m_opt;
    index = opt->hashFunc(key);
    index = index & table->m_mask;
    if(table->m_table[index] == NULL)
        table->m_used++;

    entry->m_key   = opt->keyDup(key);
    entry->m_next  = table->m_table[index];
    entry->m_value.m_ptr = opt->valDup(value);
    table->m_table[index] = entry;

    return QBARREL_SUCCESS;
}

