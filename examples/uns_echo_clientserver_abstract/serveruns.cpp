//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "serveruns.h"

#include <cstdio>
#include <cstring>

#include "clienthandler.h"

#define UNS(x) (x == AF_UNIX ? "AF_UNIX" : "unknown")
#define SERVERPATH "fealunsserver"

void Serveruns::startServer(void)
{
    feal::handle_t fd;
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    feal::sockaddr_all_t sall;
    sall.un.sun_family = AF_UNIX;
    sall.un.sun_path[0] = 0;
    strcpy(sall.un.sun_path + 1, SERVERPATH);
    unlink(SERVERPATH);
    printf("Starting Server on %s\n", SERVERPATH);
    feal::socklen_t length = sizeof(sall.un.sun_family) + strlen(SERVERPATH) + 2;
    int ret = bind(fd, &(sall.sa), length);
    if (ret != feal::FEAL_OK)
    {
        printf("Error binding to %s  err %d\n", SERVERPATH,
               static_cast<feal::errenum_t>(FEAL_GETHANDLEERRNO));
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    feal::errenum_t se = stream.listen(fd);
    if (se != feal::FEAL_OK)
    {
        printf("Error listening to %s  err %d\n", SERVERPATH, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Listening ...\n");
}

void Serveruns::printClientAddress(feal::handle_t fd)
{
    uid_t euid = 0;
    gid_t egid = 0;
    feal::getpeereid(fd, &euid, &egid);
    printf("Credentials from getpeerid: euid=%ld, egid=%ld\n", (long)euid, (long)egid);
}

void Serveruns::getClientAddress(feal::handle_t fd, char* addr, int addrbuflen)
{
    uid_t euid = 0;
    gid_t egid = 0;
    feal::getpeereid(fd, &euid, &egid);
    if (addr)
        snprintf(addr, addrbuflen, "euid=%ld, egid=%ld", (long)euid, (long)egid);
}
