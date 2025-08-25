//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "clientuns.h"

#include <cstdio>
#include <cstring>

#define SERVERPATH "/tmp/fealunsserver"

void Clientuns::connect_to_server(void)
{
    feal::handle_t fd;
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    feal::sockaddr_all sall;
    memset(&sall, 0, sizeof(sall));
    sall.un.sun_family = AF_UNIX;
    strcpy(sall.un.sun_path, SERVERPATH);
    printf("Trying to connect %s\n", SERVERPATH);
    feal::socklen_t length = sizeof(sall.un.sun_family) + strlen(sall.un.sun_path) + 1;
    feal::errenum se = stream.connect(fd, &sall, length);
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}
