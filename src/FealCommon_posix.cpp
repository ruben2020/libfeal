//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <fcntl.h>

#include "feal.h"

void feal::ipaddr_posix2feal(sockaddr_ip* su, feal::ipaddr* fa)
{
    if ((fa == nullptr) || (su == nullptr)) return;
    fa->family = su->sa.sa_family == AF_INET6 ? feal::ipaddr::INET6 : feal::ipaddr::INET;
    if (fa->family == feal::ipaddr::INET)
    {
        fa->port = ntohs(su->in.sin_port);
        inet_ntop(AF_INET, &(su->in.sin_addr), fa->addr, INET_ADDRSTRLEN);
    }
    else if (fa->family == feal::ipaddr::INET6)
    {
        fa->port = ntohs(su->in6.sin6_port);
        inet_ntop(AF_INET6, &(su->in6.sin6_addr), fa->addr, INET6_ADDRSTRLEN);
    }
}

int feal::ipaddr_feal2posix(feal::ipaddr* fa, sockaddr_ip* su)
{
    int res = -1;
    if ((fa == nullptr) || (su == nullptr)) return -1;
    su->sa.sa_family = (unsigned short) fa->family;
    if (fa->family == feal::ipaddr::INET)
    {
        su->in.sin_port = htons(fa->port);
        res = inet_pton(AF_INET, fa->addr, &(su->in.sin_addr));
    }
    else if (fa->family == feal::ipaddr::INET6)
    {
        su->in6.sin6_port = htons(fa->port);
        res = inet_pton(AF_INET6, fa->addr, &(su->in6.sin6_addr));
    }
    return res;
}

int feal::set_nonblocking(handle_t fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int feal::set_nonblocking(handle_t fd[2])
{
    int ret1 = 0;
    int ret2 = 0;
    ret1 = feal::set_nonblocking(fd[0]);
    ret2 = feal::set_nonblocking(fd[1]);
    return (ret1 < 0 ? ret1 : ret2);
}

int feal::set_ipv6only(handle_t fd)
{
    int on = 1;
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
        (char *)&on, sizeof(on)) == -1)
    {
        return -1;
    }
    return 0;
}

int feal::set_reuseaddr(handle_t fd, bool enable)
{
    int opt = enable ? 1 : 0;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, 
            &opt, sizeof(opt));
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

feal::errenum feal::getpeereid(feal::handle_t fd, uid_t* euid, gid_t* egid)
{
    errenum res = FEAL_OK;
    int ret;
#if defined (__linux__)
    socklen_t len;
    struct ucred ucred;
    len = sizeof(struct ucred);
    ret = getsockopt(fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len);
    if (ret != FEAL_HANDLE_ERROR)
    {
        *euid = (uid_t) ucred.uid;
        *egid = (gid_t) ucred.gid;
    }
#else
    ret = ::getpeereid(fd, euid, egid);
#endif
    if (ret == FEAL_HANDLE_ERROR)
    {
        res = static_cast<errenum>(FEAL_GETHANDLEERRNO);
        return res;
    }
    return res;
}

#if defined (__linux__)
int feal::epoll_ctl_add(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
}

int feal::epoll_ctl_mod(int epfd, handle_t fd, uint32_t events)
{
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
}

int feal::epoll_ctl_del(int epfd, handle_t fd)
{
    struct epoll_event ev;
    ev.events = 0;
    ev.data.fd = fd;
    return epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
}
#endif


