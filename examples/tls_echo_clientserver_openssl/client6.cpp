//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "client6.h"

void Client6::connect_to_server(void)
{
    feal::ipaddr serveraddr;
    serveraddr.family = feal::ipaddr::INET6;
    serveraddr.port = 58103;
    strcpy(serveraddr.addr, "::1");
    printf("Trying to connect ::1 port %d\n", serveraddr.port);
    feal::errenum se = stream.create_and_connect(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to ::1 port %d  err %d\n", serveraddr.port, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}


