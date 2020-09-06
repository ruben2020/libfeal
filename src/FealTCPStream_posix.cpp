#include <cstring>
#include "feal.h"

feal::TCPStream::sockerrenum feal::TCPStream::initSock(void)
{
    sockerrenum res = S_OK;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) res = static_cast<sockerrenum>(errno);
    return res;
}

feal::TCPStream::sockerrenum feal::TCPStream::bindSock(const char *ipaddr, uint16_t port)
{
    return bindSock(inet_addr(ipaddr), port);
}

feal::TCPStream::sockerrenum feal::TCPStream::bindSock(uint32_t ipaddr, uint16_t port)
{
    sockerrenum res = S_OK;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof (addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = ipaddr;
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
            res = static_cast<sockerrenum>(errno);
    return  res;
}

