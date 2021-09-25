#ifndef _FEAL_DATAGRAM_WINSOCK_H
#define _FEAL_DATAGRAM_WINSOCK_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

namespace feal
{

template<typename Y>
class Datagram : public Tool
{
public:
Datagram() = default;
Datagram( const Datagram & ) = default;
Datagram& operator= ( const Datagram & ) = default;
~Datagram() = default;

std::thread datagramThread;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}

void shutdownTool(void)
{
    close_and_reset();
}

errenum create_sock(family_t fam)
{
    errenum res = FEAL_OK;
    sockfd = socket((int) fam, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
    {
        res = static_cast<errenum>(WSAGetLastError());
        return res;
    }
    setnonblocking(sockfd);
    EvtSockErr ese;
    EvtDgramReadAvail edra;
    EvtDgramWriteAvail edwa;
    actorptr->addEvent(actorptr, ese);
    actorptr->addEvent(actorptr, edra);
    actorptr->addEvent(actorptr, edwa);
    datagramThread = std::thread(&dgramLoopLauncher, this);
    return res;
}

errenum bind_sock(feal::ipaddr* fa)
{
    errenum res = FEAL_OK;
    int ret;
    if (fa == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(fa, &su);
    if (ret == SOCKET_ERROR)
    {
        res = static_cast<errenum>(WSAGetLastError());
        return  res;
    }
    socklen_t length = sizeof(su.in);
    if (fa->family == feal::ipaddr::INET6)
    {
        setipv6only(sockfd);
        length = sizeof(su.in6);
    }
    ret = bind(sockfd, &(su.sa), length);
    if (ret == SOCKET_ERROR)
    {
        res = static_cast<errenum>(WSAGetLastError());
        return res;
    }
    return res;
}

errenum recv_from(void *buf, uint32_t len, int32_t* bytes, feal::ipaddr* src)
{
    errenum res = FEAL_OK;
    sockaddr_ip su;
    socklen_t addrlen = sizeof(su);
    memset(&su, 0, sizeof(su));
    ssize_t numbytes = recvfrom(sockfd, (char *) buf, (size_t) len, 
                0, &(su.sa), &addrlen);
    sockread[0] = sockfd;
    if (numbytes == SOCKET_ERROR)
    {
        res = static_cast<errenum>(WSAGetLastError());
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    if (src) ipaddr_posix2feal(&su, src);
    return res;
}

errenum send_to(void *buf, uint32_t len, int32_t* bytes, 
    feal::ipaddr* dest, bool confirm = false)
{
    errenum res = FEAL_OK;
    int ret;
    (void)confirm;
    if (dest == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(dest, &su);
    if (ret == SOCKET_ERROR)
    {
        res = static_cast<errenum>(WSAGetLastError());
        return  res;
    }
    socklen_t length = sizeof(su.in);
    if (dest->family == feal::ipaddr::INET6)
    {
        length = sizeof(su.in6);
    }
    ssize_t numbytes = sendto(sockfd, (char *) buf, (size_t) len,
            0, &(su.sa), length);
    if (numbytes == SOCKET_ERROR)
    {
        if (WSAGetLastError() == WSAEWOULDBLOCK)
        {
            do_send_avail_notify();
        }
        res = static_cast<errenum>(WSAGetLastError());
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

errenum close_and_reset(void)
{
    errenum res = FEAL_OK;
    if ((sockfd != INVALID_SOCKET)&&(shutdown(sockfd, SD_BOTH) == SOCKET_ERROR))
        res = static_cast<errenum>(WSAGetLastError());
    closesocket(sockfd);
    sockfd = INVALID_SOCKET;
    for (int j=0; j < max_events; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
    if (datagramThread.joinable()) datagramThread.join();
    return res;
}

private:

Y* actorptr = nullptr;
socket_t sockfd = INVALID_SOCKET;
#define FEALDGRAM_MAXEVENTS       (FD_SETSIZE > 64 ? 64 : FD_SETSIZE)
const int max_events = FEALDGRAM_MAXEVENTS;
socket_t sockread[FEALDGRAM_MAXEVENTS];
socket_t sockwrite[FEALDGRAM_MAXEVENTS];


static void dgramLoopLauncher(Datagram *p)
{
    if (p) p->dgramLoop();
}

void dgramLoop(void)
{
    int nfds = 0;
    ssize_t numbytes;
    int wsaerr;
    struct timeval tv;
    char buf[100];
    tv.tv_sec = 0;
    tv.tv_usec = 500000; // 500ms
    FD_SET ReadSet;
    FD_SET WriteSet;
    for (int j=0; j < max_events; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
    sockread[0] = sockfd;
    for (;;)
    {
        if (sockfd == INVALID_SOCKET) break;
        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);
        for (int j=0; j < max_events; j++)
        {
            if (sockread[j]  != INVALID_SOCKET) FD_SET(sockread[j],  &ReadSet);
            if (sockwrite[j] != INVALID_SOCKET) FD_SET(sockwrite[j], &WriteSet);
        }
        nfds = select(0, &ReadSet, &WriteSet, nullptr, &tv);
        //printf("select dgramLoop nfds=%d\n", nfds);
        if (nfds == 0) continue; // timeout
        if (nfds == SOCKET_ERROR)
        {
            wsaerr = WSAGetLastError();
            if (wsaerr == WSAEINVAL)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            printf("select dgramLoop nfds=%d, err=%d\n", nfds, wsaerr);
            break;
        }
        for (int i = 0; i < max_events; i++)
        {
            if (nfds <= 0) break;
            if (FD_ISSET(sockread[i], &ReadSet))
            {
                nfds--;
                numbytes = recvfrom(sockfd, buf, sizeof(buf), 
                    MSG_PEEK, nullptr, nullptr);
                if ((numbytes == SOCKET_ERROR)&&(WSAGetLastError() != WSAEWOULDBLOCK))
                {
                    close_sock();
                    sock_error();
                    break;
                }
                sockread[0] = INVALID_SOCKET;
                sock_read_avail();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            else if (FD_ISSET(sockwrite[i], &WriteSet))
            {
                nfds--;
                sockwrite[0] = INVALID_SOCKET;
                sock_write_avail();
                continue;
            }
        }
    }
}

void close_sock(void)
{
    shutdown(sockfd, SD_BOTH);
    closesocket(sockfd);
    sockfd = INVALID_SOCKET;
    for (int j=0; j < max_events; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
}

void sock_error(void)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtSockErr> evt = std::make_shared<EvtSockErr>();
    actorptr->receiveEvent(evt);
}

void sock_read_avail(void)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtDgramReadAvail> evt = std::make_shared<EvtDgramReadAvail>();
    evt.get()->sockfd = sockfd;
    evt.get()->datalen = datareadavaillen(sockfd);
    actorptr->receiveEvent(evt);
}

void sock_write_avail(void)
{
    if (actorptr == nullptr) return;
    std::shared_ptr<EvtDgramWriteAvail> evt = std::make_shared<EvtDgramWriteAvail>();
    evt.get()->sockfd = sockfd;
    actorptr->receiveEvent(evt);
}

void do_send_avail_notify(void)
{
    sockwrite[0] = sockfd;
}


};


} // namespace feal



#endif // _FEAL_DATAGRAM_WINSOCK_H
