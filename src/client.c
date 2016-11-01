
#include "btype.h"
#include "bclient.h"

QB_PUBLIC qbClient* qbclient_new(const char *ip, int port)
{
    qbClient *clnt;

    clnt = (qbClient*)qmalloc(sizeof(qbClient));
    if(clnt == NULL)
        goto clnt_Err;

    clnt->m_obj = (bObject*)qmalloc(sizeof(bObject));
    if(clnt->m_obj == NULL)
        goto obj_Err;

    clnt->m_obj->m_type = QBARREL_TYPE_LIST;
    clnt->m_obj->m_encoding = QBARREL_ENCODING_LINKDLIST;
    clnt->m_obj->m_refcount = 0;//refcount increase when socket connected
    clnt->m_obj->m_ptr = clnt;

    clnt->m_ip   = _bstrdup(ip);
    clnt->m_port = port;

    return clnt;

obj_Err:
    qfree(clnt);
clnt_Err:
    return NULL;
}

int main()
{
    qbClient *clnt;

    clnt = qbclient_new();
}
