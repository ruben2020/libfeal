//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include "feal.h"

#if defined (_WIN32)
#define BUFCAST(x) (char *) x
#define CLOSESOCKET(x) closesocket(x)
#else
#define BUFCAST(x) (void *) x
#define CLOSESOCKET(x) close(x)
#endif


void feal::DatagramGeneric::shutdownTool(void)
{
    close_and_reset();
}

void feal::DatagramGeneric::set_reuseaddr(bool enable)
{
    reuseaddr = enable;
}

feal::errenum feal::DatagramGeneric::create_sock(feal::family_t fam)
{
    errenum res = FEAL_OK;
    if (datagramThread.joinable()) return res;
    sockfd = socket((int) fam, SOCK_DGRAM, 0);
    if (sockfd == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    feal::set_reuseaddr(sockfd, reuseaddr);
    set_nonblocking(sockfd);
    datagramThread = std::thread(&dgramLoopLauncher, this);
    return res;
}

feal::errenum feal::DatagramGeneric::bind_sock(feal::ipaddr* fa)
{
    errenum res = FEAL_OK;
    int ret;
    if (fa == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(fa, &su);
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return  res;
    }
    socklen_t length = sizeof(su.in);
    if (fa->family == feal::ipaddr::INET6)
    {
        set_ipv6only(sockfd);
        length = sizeof(su.in6);
    }
    ret = bind(sockfd, &(su.sa), length);
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    return res;
}

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
feal::errenum feal::DatagramGeneric::bind_sock(struct sockaddr_un* su)
{
    errenum res = FEAL_OK;
    int ret;
    if (su == nullptr) return res;
    socklen_t length = sizeof(su->sun_family) + strlen(su->sun_path) + 1;
    ret = bind(sockfd, (const struct sockaddr*) su, length);
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    return res;
}
#endif

feal::errenum feal::DatagramGeneric::recv_from(void *buf, 
    uint32_t len, int32_t* bytes, feal::ipaddr* src)
{
    errenum res = FEAL_OK;
    sockaddr_ip su;
    socklen_t addrlen = sizeof(su);
    memset(&su, 0, sizeof(su));
    ssize_t numbytes = recvfrom(sockfd, BUFCAST(buf), (size_t) len, 
                MSG_DONTWAIT, &(su.sa), &addrlen);
#if defined (_WIN32)
    sockread[0] = sockfd;
#endif
    if (numbytes == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    if (src) ipaddr_posix2feal(&su, src);
    return res;
}

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
feal::errenum feal::DatagramGeneric::recv_from(void *buf, 
    uint32_t len, int32_t* bytes,
    struct sockaddr_un* src, socklen_t *srcaddrlen)
{
    errenum res = FEAL_OK;
    ssize_t numbytes = recvfrom(sockfd, buf, (size_t) len, 
                MSG_DONTWAIT, (struct sockaddr *) src, srcaddrlen);
    if (numbytes == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}
#endif

feal::errenum feal::DatagramGeneric::send_to(void *buf,
    uint32_t len, int32_t* bytes, 
    feal::ipaddr* dest, bool confirm)
{
    errenum res = FEAL_OK;
    int ret;
    if (dest == nullptr) return res;
    sockaddr_ip su;
    memset(&su, 0, sizeof(su));
    ret = ipaddr_feal2posix(dest, &su);
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return  res;
    }
    socklen_t length = sizeof(su.in);
    if (dest->family == feal::ipaddr::INET6)
    {
        length = sizeof(su.in6);
    }
#if defined (_WIN32)
    (void)confirm;
    int flags = 0;
#else
    int flags = ((confirm ? MSG_CONFIRM : 0) | MSG_DONTWAIT);
#endif
    ssize_t numbytes = sendto(sockfd, BUFCAST(buf), (size_t) len,
            flags, &(su.sa), length);
    if (numbytes == FEAL_HANDLE_ERROR)
    {
#if defined (_WIN32)
        if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
        if ((errno == EAGAIN)||(errno == EWOULDBLOCK))
#endif
        {
            do_send_avail_notify();
        }
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}

#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
feal::errenum feal::DatagramGeneric::send_to(void *buf, 
    uint32_t len, int32_t* bytes,
    struct sockaddr_un* dest, socklen_t destaddrlen,
    bool confirm)
{
    errenum res = FEAL_OK;
    if ((dest == nullptr)||(destaddrlen <= 0)) return res;
    int flags = ((confirm ? MSG_CONFIRM : 0) | MSG_DONTWAIT);
    ssize_t numbytes = sendto(sockfd, buf, (size_t) len,
            flags, (const struct sockaddr *) dest, destaddrlen);
    if (numbytes == FEAL_HANDLE_ERROR)
    {
        if ((errno == EAGAIN)||(errno == EWOULDBLOCK))
        {
            do_send_avail_notify();
        }
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    if (bytes) *bytes = (int32_t) numbytes;
    return res;
}
#endif

feal::errenum feal::DatagramGeneric::close_and_reset(void)
{
    errenum res = FEAL_OK;
    if ((sockfd != FEAL_INVALID_HANDLE)&&
        (shutdown(sockfd, FEAL_SHUT_RDWR) == FEAL_HANDLE_ERROR))
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
    CLOSESOCKET(sockfd);
    sockfd = FEAL_INVALID_HANDLE;
#if defined (_WIN32)
    for (int j=0; j < FEALDGRAM_MAXEVENTS; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
#elif defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
    if (datagramThread.joinable()) datagramThread.join();
    return res;
}

void feal::DatagramGeneric::dgramLoopLauncher(feal::DatagramGeneric *p)
{
    if (p) p->dgramLoop();
}

void feal::DatagramGeneric::dgramLoop(void)
{
#if defined (_WIN32)
    int nfds = 0;
    ssize_t numbytes;
    int wsaerr;
    struct timeval tv;
    char buf[100];
    tv.tv_sec = 0;
    tv.tv_usec = 500000; // 500ms
    FD_SET ReadSet;
    FD_SET WriteSet;
    for (int j=0; j < FEALDGRAM_MAXEVENTS; j++)
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
        for (int j=0; j < FEALDGRAM_MAXEVENTS; j++)
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
        for (int i = 0; i < FEALDGRAM_MAXEVENTS; i++)
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
                else
                {
                    sockread[0] = INVALID_SOCKET;
                    sock_read_avail();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            if (FD_ISSET(sockwrite[i], &WriteSet))
            {
                nfds--;
                sockwrite[0] = INVALID_SOCKET;
                sock_write_avail();
            }
        }
    }
#elif defined (__linux__)
    int nfds = 0;
    struct epoll_event events[FEALDGRAM_MAXEVENTS];
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
        nfds = epoll_wait(epfd, events, FEALDGRAM_MAXEVENTS, 500);
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
            if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                sock_read_avail();
            }
            if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                sock_write_avail();
                epoll_ctl_mod(epfd, sockfd, 
                    (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP));
            }
        }
    }
#else
    int nevts = 0;
    struct timespec tims;
    struct kevent change_event[2], event[FEALDGRAM_MAXEVENTS];
    memset(&change_event, 0, sizeof(change_event));
    memset(&event, 0, sizeof(event));
    tims.tv_sec = 0;
    tims.tv_nsec = 500000000; // 500ms
    kq = kqueue();
    EV_SET(change_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent EVFILT_READ err");
    }
    for (;;)
    {
        if ((sockfd == -1)||(kq == -1)) break;
        nevts = kevent(kq, nullptr, 0, event, FEALDGRAM_MAXEVENTS - 1, (const struct timespec *) &tims);
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
            if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                sock_read_avail();
            }
            if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                sock_write_avail();
                EV_SET(change_event, sockfd, EVFILT_WRITE, EV_DELETE, 0, 0, 0);
                kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr);
            }
        }
    }
#endif
}

void feal::DatagramGeneric::close_sock(void)
{
    shutdown(sockfd, FEAL_SHUT_RDWR);
    CLOSESOCKET(sockfd);
    sockfd = FEAL_INVALID_HANDLE;
#if defined (_WIN32)
    for (int j=0; j < FEALDGRAM_MAXEVENTS; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
#elif defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
}

void feal::DatagramGeneric::do_send_avail_notify(void)
{
#if defined (_WIN32)
    sockwrite[0] = sockfd;
#elif defined (__linux__)
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

void feal::DatagramGeneric::sock_error(void)
{
    receiveEventSockErr(FEAL_OK, FEAL_INVALID_HANDLE, -1);
}

void feal::DatagramGeneric::sock_read_avail(void)
{
    receiveEventReadAvail(FEAL_OK, sockfd, datareadavaillen(sockfd));
}

void feal::DatagramGeneric::sock_write_avail(void)
{
    receiveEventWriteAvail(FEAL_OK, sockfd, -1);
}

void feal::DatagramGeneric::receiveEventReadAvail(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::DatagramGeneric::receiveEventWriteAvail(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::DatagramGeneric::receiveEventSockErr(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}
