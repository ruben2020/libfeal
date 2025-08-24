//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "client6.h"

#define SERVERPORT 55002

void Client6::connect_to_server(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET6, SOCK_STREAM, 0);
    feal::set_ipv6only(fd);
    feal::sockaddr_all sall;
    memset(&sall, 0, sizeof(sall));
    sall.in6.sin6_family = AF_INET6;
    sall.in6.sin6_port = htons(SERVERPORT);
    feal::inet_pton(AF_INET6, "::1", &(sall.in6.sin6_addr));
    printf("Trying to connect to ::1:%d\n", SERVERPORT);
    feal::errenum se = stream.connect(fd, &sall, sizeof(sall));
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to ::1:%d  err %d\n", SERVERPORT, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}


