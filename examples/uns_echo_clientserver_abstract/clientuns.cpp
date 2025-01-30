//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "clientuns.h"

#define SERVERPATH "fealunsserver"

void Clientuns::connect_to_server(void)
{
    struct sockaddr_un serveraddr;
    serveraddr.sun_family = AF_UNIX;
    serveraddr.sun_path[0] = 0;
    strcpy(serveraddr.sun_path + 1, SERVERPATH);
    printf("Trying to connect %s\n", SERVERPATH);
    feal::errenum se = stream.create_and_connect(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}


