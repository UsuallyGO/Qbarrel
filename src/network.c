
#include "bnetwork.h"

static qbServer server;

static int _setReuseAddr(int fd)
{
    int yes = 1;

    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        return QBARREL_ERR_NETWORK;
    return QBARREL_SUCCESS;
}

static int _listen(int fd, struct sockaddr *sa, socklent_t len, int max)
{
    if(bind(fd, sa, len) == -1)
        goto Error;
    if(listen(fd, max) == -1)
        goto Error;

    return QBARREL_SUCCESS;

Error:
    close(fd);
    return QBARREL_ERR_NETWORK;
}

static int _setBlock(int fd, int block)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    if(flags == -1)
        return QBARREL_ERR_NETWORK;

    if(block)
        flags &= ~O_NONBLOCK;
    else
        flags |=  O_NONBLOCK;

    if(fcntl(fd, F_SETFL, flags) == -1)
        return QBARREL_ERR_NETWORK;

    return QBARREL_SUCCESS;
}

QB_PUBLIC int socket_tcpserver(const char* addr, int port, int af, int max)
{
    int s, res;
    char _port[6]; //for 65535
    struct addrinfo hints, *servinfo, *p;

    snprintf(_port, 6, "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; //set socket for binding and listenning

    if((res = getaddrinfo(addr, _port, &hints, &servinfo)) != 0)
        return QBARREL_ERR_NETWORK;

    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol))== -1)
            continue;

        if(_setReuseAddr(s) != QBARREL_SUCCESS)
            break;
        if(_listen(s, max) != QBARREL_SUCCESS)
            break;

        goto End;//establish one socket is enough
    }

    s = QBARREL_ERR_NETWORK;
End:
    if(p != NULL)
        freeaddrinfo(servinfo);
    return s;
}

QB_PRIVATE int server_listen(int port, int *fds, int *count)
{
    int index;

    if(server.m_bindcnt == 0)
        server.m_bindaddr[0] = NULL;

    for(index = 0; index < server.m_bindcnt || index ==0; index++)
    {
        if(server.m_bindaddr[index] == NULL)
        {
            fds[*count] = socket_tcpserver(NULL, port, server.m_fdmax);
            if(fds[*count] != QBARREL_ERR_NETWORK)
            {
                _setBlock(fds[*count], 1);
                (*count)++;
            }

            if(*count)
                break;
        }
    }

    if(fds[*count] == QBARREL_ERR_NETWORK)
        return QBARREL_ERR_NETWORK;

    return QBARREL_SUCCESS;
}

