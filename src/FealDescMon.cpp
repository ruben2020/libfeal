//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include "feal.h"

void feal::DescMonGeneric::shutdownTool(void)
{
    close_and_reset();
}

feal::errenum feal::DescMonGeneric::monitor(handle_t fd)
{
    errenum res = FEAL_OK;
    genfd = fd;
    if (genfd == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    setnonblocking(genfd);
    DescMonThread = std::thread(&fdmonLoopLauncher, this);
    return res;
}

feal::errenum feal::DescMonGeneric::close_and_reset(void)
{
    errenum res = FEAL_OK;
    close(genfd);
    genfd = FEAL_INVALID_HANDLE;
#if defined (__linux__)
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
#if defined (__linux__)
    int nfds = 0;
    struct epoll_event events[max_events];
    epfd = epoll_create(1);
    if (epoll_ctl_add(epfd, genfd, 
        (EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
    for (;;)
    {
        if ((genfd == -1)||(epfd == -1)) break;
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
                close_fd();
                fd_error();
                break;
            }
            else if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                fd_read_avail();
                continue;
            }
            else if ((events[i].events & EPOLLOUT) == EPOLLOUT)
            {
                fd_write_avail();
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
    EV_SET(change_event, genfd, EVFILT_READ , EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        printf("kevent err %d\n", errno);
    }
    EV_SET(change_event, genfd, EVFILT_WRITE, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        printf("kevent err %d\n", errno);
    }
    for (;;)
    {
        if ((genfd == -1)||(kq == -1)) break;
        nevts = kevent(kq, nullptr, 0, event, max_events - 1, (const struct timespec *) &tims);
        if (nevts == 0) continue;
        if (nevts == -1) break;
        for (int i = 0; i < nevts; i++)
        {
            if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                close_fd();
                fd_error();
                break;
            }
            else if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                fd_read_avail();
                continue;
            }
            else if ((event[i].filter & EVFILT_WRITE) == EVFILT_WRITE)
            {
                fd_write_avail();
                continue;
            }
        }
    }
#endif
}

void feal::DescMonGeneric::close_fd(void)
{
    close(genfd);
    genfd = FEAL_INVALID_HANDLE;
#if defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
}

#if defined (__linux__)
int feal::DescMonGeneric::epoll_ctl_add(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

int feal::DescMonGeneric::epoll_ctl_mod(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}
#endif

void feal::DescMonGeneric::fd_error(void)
{
    receiveEventDescErr(FEAL_OK, FEAL_INVALID_HANDLE, -1);
}

void feal::DescMonGeneric::fd_read_avail(void)
{
    receiveEventReadAvail(FEAL_OK, genfd, datareadavaillen(genfd));
}

void feal::DescMonGeneric::fd_write_avail(void)
{
    receiveEventWriteAvail(FEAL_OK, genfd, -1);
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
