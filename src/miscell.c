
#include <string.h>
#include "bmiscell.h"

QB_PRIVATE char* _bstrdup(const char* psz)
{
    int len;
    char* dup;

    if(psz == NULL)
        return NULL;

    len = strlen(psz);
    dup = (char*)qmalloc(len + 1);//one more byte for '\0'
    assert(dup != NULL);
    strncpy(dup, psz, len);
    dup[len] = '\0';//why strncpy and dup[len] but not strcpy? Maybe safer

    return dup;
}

