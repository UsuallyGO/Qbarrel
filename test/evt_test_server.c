
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "qbarrel.h"
#include "qevent.h"

#define IP_ADDRESS "127.0.0.1"
#define PORT 8787
#define MAX_LINE 1024
#define LISTEN 5
#define SIZE 10

void recv_msg(wrpEvtLoop *lp, int fd, int mask, void *arg)
{
    int client_fd, n;
    char buf[MAX_LINE];

    client_fd = fd;
    sprintf(buf, "Empty buffer.\n");
    n = recv(client_fd, buf, MAX_LINE, 0);
    printf("rev msg:%s\n", buf);

    evtloop_addfilevt(lp, fd, EVENT_READABLE, recv_msg, NULL);
}

void listenFunc(wrpEvtLoop *lp, int fd, int mask, void *arg)
{
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    int client_fd = -1;
    int server_fd = -1;
    int index;

    server_fd = fd;
    cliaddrlen = sizeof(cliaddr);

    client_fd = accept(server_fd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    assert(client_fd != -1);

    printf("Accept new connection:%s %d\n", inet_ntoa(cliaddr.sin_addr),\
                                          cliaddr.sin_port);

    evtloop_addfilevt(lp, client_fd, EVENT_READABLE, recv_msg, NULL);
    
    printf("file event callback.\n");
}

int main()
{
    int res, listen_fd;
    int yes, reuse;
    struct sockaddr_in serv_addr;
    wrpEvtLoop *evtlp;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    assert(listen_fd != -1);

    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        assert(0);
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        assert(0);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, IP_ADDRESS, &serv_addr.sin_addr);
    serv_addr.sin_port = htons(PORT);
    if(bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        assert(0);

    listen(listen_fd, LISTEN);
    printf("Sever fd:%d is listening...\n", listen_fd);

    evtlp = evtloop_create(100);
    assert(evtlp != NULL);

    res = evtloop_addfilevt(evtlp, listen_fd, EVENT_READABLE, listenFunc, NULL);
    assert(res == QBARREL_SUCCESS);

    evtlooping(evtlp);

    printf("input res:"), scanf("%d", &res);
    return 0;
}
