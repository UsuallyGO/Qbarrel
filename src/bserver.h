
#ifndef __BARREL_SERVER_H__
#define __BARREL_SERVER_H__

#include "qtype.h"

#define NETWORK_BIND_MAX 16

typedef struct qbServer_t qbServer;

struct qbServer_t
{
    Char* m_cfgfile;
    bObject *m_obj;

    //qbDB here...

    qDict *m_cmds;
    wrpEvtLoop *m_evtlp;

    /* Networking */
    Int m_port;
    Int m_fdmax;
    Int m_bindcnt;
    Char *m_bindaddr[NETWORK_BIND_MAX];
    Int bind_fd[NETWORK_BIND_MAX];
};

#endif //!__BARREL_SERVER_H__