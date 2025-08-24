//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "client6.h"

#define SERVERPORT 56002

void Client6::send_to_server(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    feal::set_ipv6only(fd);
    feal::errenum se;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.in6.sin6_family = AF_INET6;
    serverport = htons(SERVERPORT);
    serveraddr.in6.sin6_port = serverport;
    feal::inet_pton(AF_INET6, "::1", &(serveraddr.in6.sin6_addr));
    se = dgram.monitor_sock(fd);
    if (se != feal::FEAL_OK) printf("Error create sock: %d\n", se);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));

}

