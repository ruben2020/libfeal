//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <cstdio>
#include <cstring>
#include "clienthandler.h"
#include "serveruns.h"

#define UNS(x) (x == AF_UNIX ? "AF_UNIX" : "unknown")
#define SERVERPATH "fealunsserver"

void Serveruns::start_server(void)
{
    struct sockaddr_un serveraddr;
    serveraddr.sun_family = AF_UNIX;
    serveraddr.sun_path[0] = 0;
    strcpy(serveraddr.sun_path + 1, SERVERPATH);
    unlink(SERVERPATH);
    printf("Starting Server on %s\n", SERVERPATH);
    feal::errenum se = stream.create_and_bind(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("Error binding to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    se = stream.listen();
    if (se != feal::FEAL_OK)
    {
        printf("Error listening to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Listening ...\n");
}

void Serveruns::print_client_address(feal::socket_t fd)
{
    uid_t euid=0;
    gid_t egid=0;
    stream.getpeereid(fd, &euid, &egid);
    printf("Credentials from getpeerid: euid=%ld, egid=%ld\n",
        (long) euid, (long) egid);
}

void Serveruns::get_client_address(feal::socket_t fd, char* addr)
{

    uid_t euid=0;
    gid_t egid=0;
    stream.getpeereid(fd, &euid, &egid);
    if (addr) sprintf(addr, "euid=%ld, egid=%ld",
                (long) euid, (long) egid);
}
