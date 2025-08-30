//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <fcntl.h>

#include "feal.h"

int feal::inet_pton(int af, const char *src, void *dst)
{
    return ::inet_pton(af, src, dst);
}

const char *feal::inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    return ::inet_ntop(af, src, dst, size);
}

int feal::setNonBlocking(handle_t fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int feal::setNonBlocking(handle_t fd[2])
{
    int ret1 = 0;
    int ret2 = 0;
    ret1 = feal::setNonBlocking(fd[0]);
    ret2 = feal::setNonBlocking(fd[1]);
    return (ret1 < 0 ? ret1 : ret2);
}

int feal::setIpv6Only(handle_t fd)
{
    int on = 1;
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&on, sizeof(on)) == -1)
    {
        return -1;
    }
    return 0;
}

int feal::setReuseAddr(handle_t fd, bool enable)
{
    int opt = enable ? 1 : 0;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
}

int feal::datareadavaillen(handle_t fd)
{
    int len;
    int ret = -1;
    if (ioctl(fd, FIONREAD, &len) != -1)
    {
        ret = len;
    }
    return ret;
}

feal::errenum_t feal::getpeereid(feal::handle_t fd, uid_t *euid, gid_t *egid)
{
    errenum_t res = FEAL_OK;
    int ret;
#if defined(__linux__)
    socklen_t len;
    struct ucred ucred;
    len = sizeof(struct ucred);
    ret = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len);
    if (ret != FEAL_HANDLE_ERROR)
    {
        *euid = (uid_t)ucred.uid;
        *egid = (gid_t)ucred.gid;
    }
#else
    ret = ::getpeereid(fd, euid, egid);
#endif
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum_t>(FEAL_GETHANDLEERRNO);
        return res;
    }
    return res;
}

std::string feal::getAddr(sockaddr_all_t *sa)
{
    char arr[200];
    if (sa == nullptr)
        return std::string();
    if (sa->sa.sa_family == AF_INET)
    {
        ::inet_ntop(AF_INET, &(sa->sa), arr, 200);
    }
    else if (sa->sa.sa_family == AF_INET6)
    {
        ::inet_ntop(AF_INET6, &(sa->sa), arr, 200);
    }
    else if (sa->sa.sa_family == AF_UNIX)
    {
        snprintf(arr, 200, "%s", sa->un.sun_path);
    }
    else
    {
        arr[0] = 0;
    }
    return std::string(arr);
}

std::string feal::getPort(sockaddr_all_t *sa)
{
    char arr[10];
    if (sa == nullptr)
        return std::string();
    if (sa->sa.sa_family == AF_INET)
    {
        snprintf(arr, 10, "%d", ntohs(sa->in.sin_port));
    }
    else if (sa->sa.sa_family == AF_INET6)
    {
        snprintf(arr, 10, "%d", ntohs(sa->in6.sin6_port));
    }
    else
    {
        snprintf(arr, 10, "%d", 0);
    }
    return std::string(arr);
}

#if defined(__linux__)
int feal::epollCtlAdd(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

int feal::epollCtlMod(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}

int feal::epollCtlDel(int epfd, handle_t fd)
{
    struct epoll_event ev;
    ev.events = 0;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
}
#endif
