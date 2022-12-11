//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR LGPL-2.1-or-later
//
 
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

int feal::setnonblocking(handle_t fd)
{
	if (fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | O_NONBLOCK) ==
	    -1)
    {
		return -1;
	}
	return 0;
}

int feal::setipv6only(handle_t fd)
{
    int on = 1;
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
        (char *)&on, sizeof(on)) == -1)
    {
        return -1;
    }
    return 0;
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


