#ifndef _FEAL_DATAGRAM_POSIX_H
#define _FEAL_DATAGRAM_POSIX_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <cstring>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#if defined (__linux__)
#include <sys/epoll.h>
#else
#include <sys/event.h>
#endif

namespace feal
{

class EvtDgramReadAvail : public Event
{
public:
EvtDgramReadAvail() = default;
EvtDgramReadAvail( const EvtDgramReadAvail & ) = default;
EvtDgramReadAvail& operator= ( const EvtDgramReadAvail & ) = default;
~EvtDgramReadAvail() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtDgramReadAvail>();
}
socket_t sockfd = -1;
int datalen = -1;
};

class EvtDgramWriteAvail : public Event
{
public:
EvtDgramWriteAvail() = default;
EvtDgramWriteAvail( const EvtDgramWriteAvail & ) = default;
EvtDgramWriteAvail& operator= ( const EvtDgramWriteAvail & ) = default;
~EvtDgramWriteAvail() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtDgramWriteAvail>();
}
socket_t sockfd = -1;
};


class EvtSockErr : public Event
{
public:
EvtSockErr() = default;
EvtSockErr( const EvtSockErr & ) = default;
EvtSockErr& operator= ( const EvtSockErr & ) = default;
~EvtSockErr() = default;
EventId_t getId(void)
{
    return getIdOfType<EvtSockErr>();
}

};


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

sockerrenum create_sock(family_t fam)
{
    sockerrenum res = S_OK;
    sockfd = socket((int) fam, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        res = static_cast<sockerrenum>(errno);
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

sockerrenum bind_sock(feal::ipaddr* fa)
{
    sockerrenum res = S_OK;
    int ret;
    if (fa == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(fa, &su);
    if (ret != 1)
    {
        res = static_cast<sockerrenum>(errno);
        return  res;
    }
    socklen_t length = sizeof(su.in);
    if (fa->family == feal::ipaddr::INET6)
    {
        setipv6only(sockfd);
        length = sizeof(su.in6);
    }
    ret = bind(sockfd, &(su.sa), length);
    if (ret == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    return res;
}

sockerrenum bind_sock(struct sockaddr_un* su)
{
    sockerrenum res = S_OK;
    int ret;
    if (su == nullptr) return res;
    socklen_t length = sizeof(su->sun_family) + strlen(su->sun_path) + 1;
    ret = bind(sockfd, (const struct sockaddr*) su, length);
    if (ret == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    return res;
}

sockerrenum recv_from(void *buf, uint32_t len, int32_t* bytes, feal::ipaddr* src)
{
    sockerrenum res = S_OK;
    sockaddr_ip su;
    socklen_t addrlen = sizeof(su);
    memset(&su, 0, sizeof(su));
    ssize_t numbytes = recvfrom(sockfd, buf, (size_t) len, 
                MSG_DONTWAIT, &(su.sa), &addrlen);
    if (numbytes == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    if (src) ipaddr_posix2feal(&su, src);
    return res;
}

sockerrenum recv_from(void *buf, uint32_t len, int32_t* bytes,
    struct sockaddr_un* src, socklen_t *srcaddrlen)
{
    sockerrenum res = S_OK;
    ssize_t numbytes = recvfrom(sockfd, buf, (size_t) len, 
                MSG_DONTWAIT, (struct sockaddr *) src, srcaddrlen);
    if (numbytes == -1)
    {
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

sockerrenum send_to(void *buf, uint32_t len, int32_t* bytes, 
    feal::ipaddr* dest, bool confirm = false)
{
    sockerrenum res = S_OK;
    int ret;
    if (dest == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(dest, &su);
    if (ret != 1)
    {
        res = static_cast<sockerrenum>(errno);
        return  res;
    }
    socklen_t length = sizeof(su.in);
    if (dest->family == feal::ipaddr::INET6)
    {
        length = sizeof(su.in6);
    }
    int flags = ((confirm ? MSG_CONFIRM : 0) | MSG_DONTWAIT);
    ssize_t numbytes = sendto(sockfd, buf, (size_t) len,
            flags, &(su.sa), length);
    if (numbytes == -1)
    {
        if ((errno == EAGAIN)||(errno == EWOULDBLOCK))
        {
            do_send_avail_notify();
        }
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

sockerrenum send_to(void *buf, uint32_t len, int32_t* bytes,
    struct sockaddr_un* dest, socklen_t destaddrlen,
    bool confirm = false)
{
    sockerrenum res = S_OK;
    int ret;
    if ((dest == nullptr)||(destaddrlen <= 0)) return res;
    int flags = ((confirm ? MSG_CONFIRM : 0) | MSG_DONTWAIT);
    ssize_t numbytes = sendto(sockfd, buf, (size_t) len,
            flags, (const struct sockaddr *) dest, destaddrlen);
    if (numbytes == -1)
    {
        if ((errno == EAGAIN)||(errno == EWOULDBLOCK))
        {
            do_send_avail_notify();
        }
        res = static_cast<sockerrenum>(errno);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

sockerrenum close_and_reset(void)
{
    sockerrenum res = S_OK;
    if ((sockfd != -1)&&(shutdown(sockfd, SHUT_RDWR) == -1))
        res = static_cast<sockerrenum>(errno);
    close(sockfd);
    sockfd = -1;
#if defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
    if (datagramThread.joinable()) datagramThread.join();
    return res;
}

private:

Y* actorptr = nullptr;
socket_t sockfd = -1;
const unsigned int max_events = 64;
#if defined (__linux__)
int epfd = -1;
#else
int kq = -1;
#endif

static void dgramLoopLauncher(Datagram *p)
{
    if (p) p->dgramLoop();
}

void dgramLoop(void)
{
#if defined (__linux__)
    int nfds = 0;
    struct epoll_event events[max_events];
    epfd = epoll_create(1);
    if (epoll_ctl_add(epfd, sockfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
    for (;;)
    {
        if ((sockfd == -1)||(epfd == -1)) break;
        nfds = epoll_wait(epfd, events, max_events, 500);
        if (nfds == -1)
        {
            if (errno == EINTR) continue;
            else break;
        }
        for (int i = 0; i < nfds; i++)
        {
            if ((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0)
            {
                close_sock();
                sock_error();
                break;
            }
            else if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                sock_read_avail();
                continue;
            }
            else if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                sock_write_avail();
                epoll_ctl_mod(epfd, sockfd, 
                    (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));
                continue;
            }
        }
    }
#else
    int nevts = 0;
    struct timespec tims;
    struct kevent change_event[2], event[max_events];
    memset(&change_event, 0, sizeof(change_event));
    memset(&event, 0, sizeof(event));
    tims.tv_sec = 0;
    tims.tv_nsec = 500000000; // 500ms
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        printf("kevent err %d\n", errno);
    }
    for (;;)
    {
        if ((sockfd == -1)||(kq == -1)) break;
        nevts = kevent(kq, nullptr, 0, event, max_events - 1, (const struct timespec *) &tims);
        if (nevts == 0) continue;
        if (nevts == -1) break;
        for (int i = 0; i < nevts; i++)
        {
            if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                close_sock();
                sock_error();
                break;
            }
            else if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                sock_read_avail();
                continue;
            }
            else if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                sock_write_avail();
                EV_SET(change_event, sockfd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
                kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
                continue;
            }
        }
    }
#endif
}

void close_sock(void)
{
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
    sockfd = -1;
#if defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
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
#if defined (__linux__)
    if (epoll_ctl_mod(epfd, sockfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLOUT)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
#else
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, sockfd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
#endif
}

#if defined (__linux__)
static int epoll_ctl_add(int epfd, socket_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

static int epoll_ctl_mod(int epfd, socket_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}
#endif

};


} // namespace feal


#endif // _FEAL_DATAGRAM_POSIX_H
