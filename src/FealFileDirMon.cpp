//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

void feal::FileDirMonGeneric::shutdownTool(void)
{
    close_and_reset();
}

feal::errenum feal::FileDirMonGeneric::monitor(const char *s, flags_t mask)
{
    errenum res = FEAL_OK;
    fdname = s;
    fdmask = mask;
#if defined (__linux__)
    genfd = inotify_init();
    if (genfd == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    genwd = inotify_add_watch(genfd, fdname.c_str(), fdmask);
#else
    genfd = open(fdname.c_str(), O_RDONLY);
    if (genfd == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
#endif
    setnonblocking(genfd);
    FileDirMonThread = std::thread(&fdmonLoopLauncher, this);
    return res;
}

feal::errenum feal::FileDirMonGeneric::close_and_reset(void)
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
    if (FileDirMonThread.joinable()) FileDirMonThread.join();
    return res;
}

void feal::FileDirMonGeneric::fdmonLoopLauncher(feal::FileDirMonGeneric *p)
{
    if (p) p->fdmonLoop();
}

void feal::FileDirMonGeneric::fdmonLoop(void)
{
#if defined (__linux__)
    int nfds = 0;
    struct epoll_event events[max_events];
    epfd = epoll_create(1);
    if (epoll_ctl_add(epfd, genfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
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
                size_t bufsize = sizeof(struct inotify_event) + NAME_MAX + 1;
                struct inotify_event* event = new struct inotify_event[bufsize / sizeof(struct inotify_event)];
                if (read(genfd, event, bufsize) > 0)
                {
                    fd_read_avail(event[0].mask);
                }
                delete[] event;
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
    EV_SET(change_event, genfd, EVFILT_VNODE , EV_ADD | EV_ENABLE | EV_CLEAR, fdmask, 0, 0);
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
            else if ((event[i].filter & EVFILT_VNODE) == EVFILT_VNODE)
            {
                fd_read_avail(event[i].fflags);
                continue;
            }
        }
    }
#endif
}

void feal::FileDirMonGeneric::close_fd(void)
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

void feal::FileDirMonGeneric::fd_error(void)
{
    receiveEventDescErr(FEAL_OK, FEAL_INVALID_HANDLE, -1, 0);
}

void feal::FileDirMonGeneric::fd_read_avail(flags_t flags1)
{
    receiveEventReadAvail(FEAL_OK, genfd, datareadavaillen(genfd), flags1);
}

void feal::FileDirMonGeneric::receiveEventReadAvail(errenum errnum, handle_t fd, int datalen, flags_t flags1)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
    (void) flags1;
}

void feal::FileDirMonGeneric::receiveEventDescErr(errenum errnum, handle_t fd, int datalen, flags_t flags1)
{
    (void) errnum;
    (void) fd;
    (void) datalen;
    (void) flags1;
}
