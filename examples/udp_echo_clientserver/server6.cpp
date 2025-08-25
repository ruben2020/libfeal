//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "server6.h"

#include <cstdio>
#include <cstring>

#define SERVERPORT 56002

void Server6::start_listening(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    feal::set_ipv6only(fd);
    feal::sockaddr_all sall;
    memset(&sall, 0, sizeof(sall));
    sall.in6.sin6_family = AF_INET6;
    sall.in6.sin6_port = htons(SERVERPORT);
    feal::inet_pton(AF_INET6, "::1", &(sall.in6.sin6_addr));
    feal::errenum se;
    se = dgram.monitor_sock(fd);
    if (se != feal::FEAL_OK)
    {
        se = static_cast<feal::errenum>(FEAL_GETHANDLEERRNO);
        printf("Err create sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    int ret = bind(fd, &(sall.sa), sizeof(sall));
    if (ret != feal::FEAL_OK)
    {
        se = static_cast<feal::errenum>(FEAL_GETHANDLEERRNO);
        printf("Err bind sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("UDP Listening on %s:%d ...\n", "::1", SERVERPORT);
}
