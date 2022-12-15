//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include <cstdio>
#include <cstring>
#include "clientuns.h"

#define SERVERPATH "/tmp/fealunsserver"

void Clientuns::connect_to_server(void)
{
    struct sockaddr_un serveraddr;
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SERVERPATH);
    printf("Trying to connect %s\n", SERVERPATH);
    feal::errenum se = stream.create_and_connect(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}


