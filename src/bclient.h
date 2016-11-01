
#ifndef __BARREL_CLIENT_H__
#define __BARREL_CLIENT_H__

typedef struct qbClient_t qbClient;

struct qbClient_t
{
    Char *m_ip;
    Int   m_port;
    Int   m_skfd;

    bObject *m_obj;    
};

#endif //!__BARREL_CLIENT_H__
