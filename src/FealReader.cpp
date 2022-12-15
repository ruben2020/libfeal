//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//

#include "feal.h"


void feal::ReaderGeneric::shutdownTool(void)
{
    close_and_reset();
}

feal::errenum feal::ReaderGeneric::registerhandle(handle_t fd)
{
    errenum res = FEAL_OK;
    if (fd == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    setnonblocking(fd);
    readerfd = fd;
    readerThread = std::thread(&readerLoopLauncher, this);
    return res;
}

feal::errenum feal::ReaderGeneric::close_and_reset(void)
{
    errenum res = FEAL_OK;
    if (readerfd != FEAL_INVALID_HANDLE)
        shutdown(readerfd, FEAL_SHUT_RDWR);
    close(readerfd);
    readerfd = FEAL_INVALID_HANDLE;
#if defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
    if (readerThread.joinable()) readerThread.join();
    return res;
}

void feal::ReaderGeneric::readerLoopLauncher(feal::ReaderGeneric *p)
{
    if (p) p->readerLoop();
}

void feal::ReaderGeneric::readerLoop(void)
{
#if defined (__linux__)
    int nfds = 0;
    struct epoll_event events[max_events];
    epfd = epoll_create(1);
    if (epoll_ctl_add(epfd, readerfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("epoll_ctl error");
        return;
    }
    for (;;)
    {
        if ((readerfd == -1)||(epfd == -1)) break;
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
                close_handle();
                handle_error();
                break;
            }
            else if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                handle_read_avail();
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
    EV_SET(change_event, readerfd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_CLEAR, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        printf("kevent err %d\n", errno);
    }
    for (;;)
    {
        if ((readerfd == -1)||(kq == -1)) break;
        nevts = kevent(kq, nullptr, 0, event, max_events - 1, (const struct timespec *) &tims);
        if (nevts == 0) continue;
        if (nevts == -1) break;
        for (int i = 0; i < nevts; i++)
        {
            if ((event[i].flags & (EV_EOF | EV_ERROR)) != 0)
            {
                close_handle();
                handle_error();
                break;
            }
            else if ((event[i].filter & EVFILT_READ) == EVFILT_READ)
            {
                handle_read_avail();
                continue;
            }
        }
    }
#endif
}

void feal::ReaderGeneric::close_handle(void)
{
    shutdown(readerfd, FEAL_SHUT_RDWR);
    close(readerfd);
    readerfd = FEAL_INVALID_HANDLE;
#if defined (__linux__)
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
}

#if defined (__linux__)
int feal::ReaderGeneric::epoll_ctl_add(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

int feal::ReaderGeneric::epoll_ctl_mod(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}
#endif

void feal::ReaderGeneric::handle_error(void)
{
    receiveEventSockErr(FEAL_OK, FEAL_INVALID_HANDLE, -1);
}

void feal::ReaderGeneric::handle_read_avail(void)
{
    receiveEventReadAvail(FEAL_OK, readerfd, datareadavaillen(sockfd));
}

void feal::ReaderGeneric::receiveEventReadAvail(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}

void feal::ReaderGeneric::receiveEventSockErr(errenum errnum, handle_t fd, int datalen)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
}
