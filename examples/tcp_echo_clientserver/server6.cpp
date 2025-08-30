//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "server6.h"

#include <cstdio>
#include <cstring>

#include "clienthandler.h"

#define SERVERPORT 55002

void Server6::startServer(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET6, SOCK_STREAM, 0);
    feal::sockaddr_all_t sall;
    memset(&sall, 0, sizeof(sall));
    sall.in6.sin6_family = AF_INET6;
    sall.in6.sin6_port = htons(SERVERPORT);
    feal::inet_pton(AF_INET6, "::1", &(sall.in6.sin6_addr));
    feal::setIpv6Only(fd);
    feal::setReuseAddr(fd, true);
    printf("Starting Server on 127.0.0.1:%d\n", SERVERPORT);
    int ret = bind(fd, (sockaddr*)&(sall.in6), sizeof(sall.in6));
    if (ret != feal::FEAL_OK)
    {
        printf("Error binding to 127.0.0.1:%d  err %d\n", SERVERPORT,
               static_cast<feal::errenum_t>(FEAL_GETHANDLEERRNO));
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    feal::errenum_t se = stream.listen(fd);
    if (se != feal::FEAL_OK)
    {
        printf("Error listening to 127.0.0.1:%d  err %d\n", SERVERPORT, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Listening ...\n");
}
