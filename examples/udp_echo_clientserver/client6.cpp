//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "client6.h"

#include <cstdio>
#include <cstring>

#define SERVERPORT 56002

void Client6::sendToServer(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET6, SOCK_DGRAM, 0);
    feal::setIpv6Only(fd);
    feal::errenum_t se;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.in6.sin6_family = AF_INET6;
    serverport = htons(SERVERPORT);
    serveraddr.in6.sin6_port = serverport;
    feal::inet_pton(AF_INET6, "::1", &(serveraddr.in6.sin6_addr));
    se = dgram.monitorSock(fd);
    if (se != feal::FEAL_OK)
        printf("Error create sock: %d\n", se);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));
}
