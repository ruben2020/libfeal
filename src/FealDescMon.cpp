//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include "feal.h"

void feal::DescMonGeneric::shutdownTool(void)
{
    close_and_reset();
}

void feal::DescMonGeneric::init(void)
{
#if defined (_WIN32)
    active = true;
    for (int j=0; j < max_events; j++)
    {
        sockread[j]  = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
#elif defined (__linux__)
    epfd = epoll_create(1);
#else
    kq = kqueue();
#endif
}

feal::errenum feal::DescMonGeneric::start_monitoring(void)
{
    errenum res = FEAL_OK;
    if (DescMonThread.joinable()) return res;
    DescMonThread = std::thread(&fdmonLoopLauncher, this);
    return res;
}

feal::errenum feal::DescMonGeneric::add(handle_t fd)
{
    errenum res = FEAL_OK;
    setnonblocking(fd);
#if defined (_WIN32)
    for (int j=0; j < max_events; j++)
    {
        if (sockread[j] == INVALID_SOCKET)
        {
            sockread[j]  = fd;
            sockwrite[j] = fd;
            break;
        }
    }
#elif defined (__linux__)
    if (epoll_ctl_add(epfd, fd, 
        (EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
    }
#else
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, fd, EVFILT_READ , EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent EVFILT_READ err");
    }
    EV_SET(change_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent EVFILT_WRITE err");
    }
#endif
    return res;
}

feal::errenum feal::DescMonGeneric::remove(handle_t fd)
{
    errenum res = FEAL_OK;
#if defined (_WIN32)
    for (int j=0; j < max_events; j++)
    {
        if (sockread[j] == fd)
        {
            sockread[j]  = INVALID_SOCKET;
            sockwrite[j] = INVALID_SOCKET;
            break;
        }
    }
#elif defined (__linux__)
    if (epoll_ctl_del(epfd, fd) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
    }
#else
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, fd, EVFILT_READ , EV_DELETE | EV_DISABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent err EVFILT_READ");
    }
    EV_SET(change_event, fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent err EVFILT_WRITE");
    }
#endif
    return res;
}

feal::errenum feal::DescMonGeneric::close_and_reset(void)
{
    errenum res = FEAL_OK;
#if defined (_WIN32)
    active = false;
#elif defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
    if (DescMonThread.joinable()) DescMonThread.join();
    return res;
}

void feal::DescMonGeneric::fdmonLoopLauncher(feal::DescMonGeneric *p)
{
    if (p) p->fdmonLoop();
}

void feal::DescMonGeneric::fdmonLoop(void)
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
    for (;;)
    {
        if (active == false) break;
        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);
        for (int j=0; j < max_events; j++)
        {
            if (sockread[j]  != INVALID_SOCKET) FD_SET(sockread[j],  &ReadSet);
            if (sockwrite[j] != INVALID_SOCKET) FD_SET(sockwrite[j], &WriteSet);
        }
        nfds = select(0, &ReadSet, &WriteSet, nullptr, &tv);
        if (nfds == 0) continue; // timeout
        if (nfds == SOCKET_ERROR)
        {
            wsaerr = WSAGetLastError();
            if (wsaerr == WSAEINVAL)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            FEALDEBUGLOG("select fdmonLoop");
            break;
        }
        for (int i = 0; i < max_events; i++)
        {
            if (nfds <= 0) break;
            if (FD_ISSET(sockread[i], &ReadSet))
            {
                nfds--;
                numbytes = recvfrom(sockread[i], buf, sizeof(buf), 
                    MSG_PEEK, nullptr, nullptr);
                if ((numbytes == SOCKET_ERROR)&&(WSAGetLastError() != WSAEWOULDBLOCK))
                {
                    fd_error(sockread[i]);
                    break;
                }
                else
                {
                    fd_read_avail(sockread[i]);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            if (FD_ISSET(sockwrite[i], &WriteSet))
            {
                nfds--;
                fd_write_avail(sockwrite[i]);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
#elif defined (__linux__)
    int nfds = 0;
    struct epoll_event events[max_events];
    for (;;)
    {
        if (epfd == -1) break;
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
                fd_error(events[i].data.fd);
                break;
            }
            if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                fd_read_avail(events[i].data.fd);
            }
            if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                fd_write_avail(events[i].data.fd);
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
    for (;;)
    {
        if (kq == -1) break;
        nevts = kevent(kq, nullptr, 0, event, max_events - 1, (const struct timespec *) &tims);
        if (nevts == 0) continue;
        if (nevts == -1) break;
        for (int i = 0; i < nevts; i++)
        {
            if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                fd_error(event[i].ident);
                break;
            }
            if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                fd_read_avail(event[i].ident);
            }
            if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                fd_write_avail(event[i].ident);
            }
        }
    }
#endif
}

void feal::DescMonGeneric::fd_error(handle_t fd)
{
    receiveEventDescErr(FEAL_OK, fd, -1);
}

void feal::DescMonGeneric::fd_read_avail(handle_t fd)
{
    receiveEventReadAvail(FEAL_OK, fd, datareadavaillen(fd));
}

void feal::DescMonGeneric::fd_write_avail(handle_t fd)
{
    receiveEventWriteAvail(FEAL_OK, fd, -1);
}

void feal::DescMonGeneric::receiveEventReadAvail(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::DescMonGeneric::receiveEventWriteAvail(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::DescMonGeneric::receiveEventDescErr(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}
