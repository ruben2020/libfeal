//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

void feal::DescMonGeneric::shutdownTool(void)
{
    closeAndReset();
}

void feal::DescMonGeneric::init(void)
{
#if defined(_WIN32)
    active = true;
    for (int j = 0; j < FEALDESCMON_MAXEVENTS; j++)
    {
        sockread[j] = INVALID_SOCKET;
        sockwrite[j] = INVALID_SOCKET;
    }
#elif defined(__linux__)
    epfd = epoll_create(1);
#else
    kq = kqueue();
#endif
}

feal::errenum_t feal::DescMonGeneric::startMonitoring(void)
{
    errenum_t res = FEAL_OK;
    if (DescMonThread.joinable())
        return res;
    DescMonThread = std::thread(&fdmonLoopLauncher, this);
    return res;
}

feal::errenum_t feal::DescMonGeneric::add(handle_t fd)
{
    errenum_t res = FEAL_OK;
    setNonBlocking(fd);
#if defined(_WIN32)
    for (int j = 0; j < FEALDESCMON_MAXEVENTS; j++)
    {
        if (sockread[j] == INVALID_SOCKET)
        {
            sockread[j] = fd;
            sockwrite[j] = fd;
            break;
        }
    }
#elif defined(__linux__)
    if (epollCtlAdd(epfd, fd, (EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
    }
#else
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent EVFILT_READ err");
    }
    EV_SET(change_event, fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent EVFILT_WRITE err");
    }
#endif
    return res;
}

feal::errenum_t feal::DescMonGeneric::remove(handle_t fd)
{
    errenum_t res = FEAL_OK;
#if defined(_WIN32)
    for (int j = 0; j < FEALDESCMON_MAXEVENTS; j++)
    {
        if (sockread[j] == fd)
        {
            sockread[j] = INVALID_SOCKET;
            sockwrite[j] = INVALID_SOCKET;
            break;
        }
    }
#elif defined(__linux__)
    if (epollCtlDel(epfd, fd) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
    }
#else
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, fd, EVFILT_READ, EV_DELETE | EV_DISABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent err EVFILT_READ");
    }
    EV_SET(change_event, fd, EVFILT_WRITE, EV_DELETE | EV_DISABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent *)change_event, 1, nullptr, 0, nullptr) == -1)
    {
        FEALDEBUGLOG("kevent err EVFILT_WRITE");
    }
#endif
    return res;
}

feal::errenum_t feal::DescMonGeneric::closeAndReset(void)
{
    errenum_t res = FEAL_OK;
#if defined(_WIN32)
    active = false;
#elif defined(__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
    if (DescMonThread.joinable())
        DescMonThread.join();
    return res;
}

void feal::DescMonGeneric::fdmonLoopLauncher(feal::DescMonGeneric *p)
{
    if (p)
        p->fdmonLoop();
}

void feal::DescMonGeneric::fdmonLoop(void)
{
#if defined(_WIN32)
    int nfds = 0;
    ssize_t numbytes;
    int wsaerr;
    struct timeval tv;
    char buf[100];
    tv.tv_sec = 0;
    tv.tv_usec = 500000;  // 500ms
    FD_SET ReadSet;
    FD_SET WriteSet;
    for (;;)
    {
        if (active == false)
            break;
        FD_ZERO(&ReadSet);
        FD_ZERO(&WriteSet);
        for (int j = 0; j < FEALDESCMON_MAXEVENTS; j++)
        {
            if (sockread[j] != INVALID_SOCKET)
                FD_SET(sockread[j], &ReadSet);
            if (sockwrite[j] != INVALID_SOCKET)
                FD_SET(sockwrite[j], &WriteSet);
        }
        nfds = select(0, &ReadSet, &WriteSet, nullptr, &tv);
        if (nfds == 0)
            continue;  // timeout
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
        for (int i = 0; i < FEALDESCMON_MAXEVENTS; i++)
        {
            if (nfds <= 0)
                break;
            if (FD_ISSET(sockread[i], &ReadSet))
            {
                nfds--;
                numbytes = recvfrom(sockread[i], buf, sizeof(buf), MSG_PEEK, nullptr, nullptr);
                if ((numbytes == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK))
                {
                    fdError(sockread[i]);
                    break;
                }
                else
                {
                    fdReadAvail(sockread[i]);
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            if (FD_ISSET(sockwrite[i], &WriteSet))
            {
                nfds--;
                fdWriteAvail(sockwrite[i]);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }
    }
#elif defined(__linux__)
    int nfds = 0;
    struct epoll_event events[FEALDESCMON_MAXEVENTS];
    for (;;)
    {
        if (epfd == -1)
            break;
        nfds = epoll_wait(epfd, events, FEALDESCMON_MAXEVENTS, 500);
        if (nfds == -1)
        {
            if (errno == EINTR)
                continue;
            else
                break;
        }
        for (int i = 0; i < nfds; i++)
        {
            if ((events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) != 0)
            {
                fdError(events[i].data.fd);
                break;
            }
            if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                fdReadAvail(events[i].data.fd);
            }
            if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                fdWriteAvail(events[i].data.fd);
            }
        }
    }
#else
    int nevts = 0;
    struct timespec tims;
    struct kevent change_event[2], event[FEALDESCMON_MAXEVENTS];
    memset(&change_event, 0, sizeof(change_event));
    memset(&event, 0, sizeof(event));
    tims.tv_sec = 0;
    tims.tv_nsec = 500000000;  // 500ms
    for (;;)
    {
        if (kq == -1)
            break;
        nevts = kevent(kq, nullptr, 0, event, FEALDESCMON_MAXEVENTS - 1,
                       (const struct timespec *)&tims);
        if (nevts == 0)
            continue;
        if (nevts == -1)
            break;
        for (int i = 0; i < nevts; i++)
        {
            if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                fdError(event[i].ident);
                break;
            }
            if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                fdReadAvail(event[i].ident);
            }
            if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                fdWriteAvail(event[i].ident);
            }
        }
    }
#endif
}

void feal::DescMonGeneric::fdError(handle_t fd)
{
    receiveEventDescErr(FEAL_OK, fd, -1);
}

void feal::DescMonGeneric::fdReadAvail(handle_t fd)
{
    receiveEventReadAvail(FEAL_OK, fd, datareadavaillen(fd));
}

void feal::DescMonGeneric::fdWriteAvail(handle_t fd)
{
    receiveEventWriteAvail(FEAL_OK, fd, -1);
}

void feal::DescMonGeneric::receiveEventReadAvail(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::DescMonGeneric::receiveEventWriteAvail(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}

void feal::DescMonGeneric::receiveEventDescErr(errenum_t errnum, handle_t fd, int datalen)
{
    (void)errnum;
    (void)fd;
    (void)datalen;
}
