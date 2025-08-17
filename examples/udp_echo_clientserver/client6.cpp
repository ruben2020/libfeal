//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "client6.h"

void Client6::send_to_server(void)
{
    feal::errenum se;
    serveraddr.family = feal::ipaddr::INET6;
    serveraddr.port = 56002;
    strcpy(serveraddr.addr, "::1");
    se = dgram.create_sock((feal::family_t) serveraddr.family);
    if (se != feal::FEAL_OK) printf("create sock: %d\n", se);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));

}

