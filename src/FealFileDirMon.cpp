//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

void feal::FileDirMonGeneric::shutdownTool(void)
{
    close_and_reset();
}

void feal::FileDirMonGeneric::init(void)
{
#if defined (__linux__)
    epfd = epoll_create(1);
    genfd = inotify_init();
    if (genfd == FEAL_INVALID_HANDLE)
    {
        FEALDEBUGLOG("FileDirMonGeneric::init inotify_init");
        return;
    }
    setnonblocking(genfd);
    if (epoll_ctl_add(epfd, genfd, 
        (EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP)) == -1)
    {
        FEALDEBUGLOG("FileDirMonGeneric::init epoll_ctl error");
        return;
    }
#else
    kq = kqueue();
#endif
}

feal::errenum feal::FileDirMonGeneric::start_monitoring(void)
{
    errenum res = FEAL_OK;
    if (FileDirMonThread.joinable()) return res;
    FileDirMonThread = std::thread(&fdmonLoopLauncher, this);
    return res;
}

feal::errenum feal::FileDirMonGeneric::add(const char *s, flags_t mask, handle_t *wnum)
{
    errenum res = FEAL_OK;
    handle_t wn = FEAL_INVALID_HANDLE;
#if defined (__linux__)
    wn = inotify_add_watch(genfd, s, mask);
    if (wnum) *wnum = wn;
    if (wn == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
#else
    wn = open(s, O_RDONLY);
    if (wnum) *wnum = wn;
    if (wn == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    setnonblocking(wn);
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, wn, EVFILT_VNODE , EV_ADD | EV_ENABLE | EV_CLEAR, mask, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
#endif
    if (wn != FEAL_INVALID_HANDLE)
    {
        fnmap[wn] = std::string(s);
    }
    return res;
}

feal::errenum feal::FileDirMonGeneric::remove(handle_t wnum)
{
    errenum res = FEAL_OK;
#if defined (__linux__)
    int ret = inotify_rm_watch(genfd, wnum);
    if (ret == FEAL_INVALID_HANDLE)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
#else
    struct kevent change_event[2];
    memset(&change_event, 0, sizeof(change_event));
    EV_SET(change_event, wnum, EVFILT_VNODE , EV_DELETE | EV_DISABLE, 0, 0, 0);
    if (kevent(kq, (const struct kevent	*) change_event, 1, nullptr, 0, nullptr) == -1)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        close(wnum);
        return res;
    }
    close(wnum);
#endif
    if (wnum != FEAL_INVALID_HANDLE)
    {
        fnmap.erase(wnum);
    }
    return res;
}

std::string feal::FileDirMonGeneric::get_filename(handle_t wnum)
{
    std::string fn;
    if (fnmap.find(wnum) != fnmap.end())
    {
        fn = fnmap[wnum];
    }
    else
    {
        fn = "unknown";
    }
    return fn;
}

feal::errenum feal::FileDirMonGeneric::close_and_reset(void)
{
    errenum res = FEAL_OK;
#if defined (__linux__)
    close(genfd);
    genfd = FEAL_INVALID_HANDLE;
    close(epfd);
    epfd = -1;
#else
    close(kq);
    kq = -1;
#endif
    fnmap.clear();
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
    size_t bufsize = sizeof(struct inotify_event) + NAME_MAX + 1;
    struct inotify_event* event = new struct inotify_event[bufsize / sizeof(struct inotify_event)];
    ssize_t bytesread;
    int numofrecs;
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
                fd_error();
                break;
            }
            else if ((events[i].events & EPOLLIN) == EPOLLIN)
            {
                bytesread = read(genfd, event, bufsize);
                numofrecs = bytesread / sizeof(struct inotify_event);
                if (bytesread > 0)
                for(int j=0; j<numofrecs; j++)
                {
                    fd_read_avail(event[j].wd, event[j].mask);
                }
                continue;
            }
        }
    }
    delete[] event;
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
                fd_error();
                break;
            }
            else if ((event[i].filter & EVFILT_VNODE) == EVFILT_VNODE)
            {
                fd_read_avail(event[i].ident, event[i].fflags);
                continue;
            }
        }
    }
#endif
}

void feal::FileDirMonGeneric::fd_error(void)
{
    receiveEventDescErr(FEAL_OK, FEAL_INVALID_HANDLE, -1, 0);
}

void feal::FileDirMonGeneric::fd_read_avail(handle_t fd1, flags_t flags1)
{
    receiveEventReadAvail(FEAL_OK, fd1, -1, flags1);
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
