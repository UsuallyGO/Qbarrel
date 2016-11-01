
#include <string.h>

#include "bmemory.h"
#include "bstring.h"
#include "btype.h"

#include "qbarrel.h"

QB_PRIVATE int bstr_len(const bString *bstr)
{
    if(bstr == NULL)
        return QBARREL_ERR_OBJNULL;

    return bstr->m_len;
}

//string is empty is not the same as string is NULL
QB_PRIVATE int bstr_empty(const bString *bstr)
{
    if(bstr == NULL)
        return QBARREL_ERR_OBJNULL;

    if(bstr->m_len == 0)
        return QBARREL_YES;
    else
        return QBARREL_NO;    
}

QB_PRIVATE bString* bstr_new(const char* psz)
{
    bString *bstr;
    int len;

    len = 0;
    if(psz != NULL)
        len = strlen(psz);

    bstr = qmalloc(sizeof(bString));
    if(bstr != NULL)
    {
        bstr->m_buf = (char*)qmalloc(len+1);//one more for '\0'
        if(bstr->m_buf != NULL)
        {
            strncpy(bstr->m_buf, psz, len);
            bstr->m_buf[len] = '\0';//this is necessary

            bstr->m_len  = len;
            bstr->m_size = len + 1;
        }
        else
        {
            bstr->m_len  = 0;
            bstr->m_size = 0;
        }
    } 

    return bstr;
}

//we can't use bstr = bstr_new(dup->m_buf) because we want a duplicate
//bString, in m_len, m_size and m_buf, bstr_new can not guarantee this
QB_PRIVATE bString* bstr_dup(const bString *dup)
{
    bString *bstr;

    if(dup == NULL)
        return NULL;

    bstr = (bString*)qmalloc(sizeof(bString));
    if(bstr != NULL)
    {
        bstr->m_len  = 0;
        bstr->m_size = 0;
        bstr->m_buf  = NULL;

        if(dup->m_buf != NULL)
        {
            bstr->m_buf = (char*)qmalloc(dup->m_size);
            if(bstr->m_buf != NULL)
            {
                strncpy(bstr->m_buf, dup->m_buf, dup->m_len);
                bstr->m_buf[dup->m_len] = '\0';

                bstr->m_len  = dup->m_len;
                bstr->m_size = dup->m_size;
            }
        }
    }

    return bstr;
}

QB_PRIVATE void bstr_destroy(bString *bstr)
{
    if(bstr != NULL)
    {
        if(bstr->m_buf != NULL)
            qfree(bstr->m_buf);
        qfree(bstr);
    }
}

QB_PRIVATE int bstr_set(bString *bstr, const char *psz)
{
    int len, size;

    if(bstr == NULL || psz == NULL)
        return QBARREL_ERR_INVAL;

    len = strlen(psz);
    if(bstr->m_size < len+1)
    {
        bstr->m_buf = (char*)qrealloc(bstr->m_buf, len+1);
        if(bstr->m_buf == NULL)
            return QBARREL_ERR_NOMEM;

        bstr->m_size = len + 1;
    }

    bstr->m_len = len;
    memset(bstr->m_buf, 0, bstr->m_size);
    strncpy(bstr->m_buf, psz, len);
    bstr->m_buf[len] = '\0';

    return QBARREL_SUCCESS;
}

QB_PRIVATE char* bstr_psz(const bString *str)
{
    if(str != NULL)
        return str->m_buf;
    else
        return NULL;
}
