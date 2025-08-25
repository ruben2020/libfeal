//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "server.h"

#include <cstdio>
#include <cstring>

#define SERVERPORT 56001
#define MIN(a, b) (a < b ? a : b)

void Server::initActor(void)
{
    printf("Server::initActor\n");
    timers.init(this);
    dgram.init(this);
    dgram.subscribeReadAvail<EvtDgramReadAvail>();
    dgram.subscribeWriteAvail<EvtDgramWriteAvail>();
    dgram.subscribeSockErr<EvtSockErr>();
}

void Server::startActor(void)
{
    printf("Server::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(15));
    start_listening();
}

void Server::pauseActor(void)
{
    printf("Server::pauseActor\n");
}

void Server::shutdownActor(void)
{
    printf("Server::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("Client shutdown complete\n");
}

void Server::start_listening(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    feal::sockaddr_all sall;
    memset(&sall, 0, sizeof(sall));
    sall.in.sin_family = AF_INET;
    sall.in.sin_port = htons(SERVERPORT);
    feal::inet_pton(AF_INET, "127.0.0.1", &(sall.in.sin_addr));
    feal::errenum se;
    se = dgram.monitor_sock(fd);
    if (se != feal::FEAL_OK)
    {
        se = static_cast<feal::errenum>(FEAL_GETHANDLEERRNO);
        printf("Err create sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    int ret = bind(fd, &(sall.sa), sizeof(sall));
    if (ret != feal::FEAL_OK)
    {
        se = static_cast<feal::errenum>(FEAL_GETHANDLEERRNO);
        printf("Err bind sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("UDP Listening on %s:%d ...\n", "127.0.0.1", SERVERPORT);
}

void Server::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    shutdown();
}

void Server::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtRetryTimer\n");
    start_listening();
}

void Server::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtDgramReadAvail\n");
    char buf[50];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::sockaddr_all recvaddr;
    feal::socklen_t addrsize = sizeof(recvaddr);
    feal::errenum se = dgram.recvfrom((void*)buf, sizeof(buf), &bytes, &recvaddr, &addrsize);
    if (se != feal::FEAL_OK)
    {
        se = static_cast<feal::errenum>(FEAL_GETHANDLEERRNO);
        printf("Error receiving: %d\n", se);
    }
    else
    {
        printf("Received %lld bytes: \"%s\" from %s:%s\n", (long long int)bytes, buf,
               feal::get_addr(&recvaddr).c_str(), feal::get_port(&recvaddr).c_str());
        printf("Sending back \"%s\" to %s:%s\n", buf, feal::get_addr(&recvaddr).c_str(),
               feal::get_port(&recvaddr).c_str());
        se = dgram.sendto((void*)buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, &recvaddr,
                          sizeof(recvaddr));
        if (se != feal::FEAL_OK)
            printf("Error sending back \"%s\" to %s:%s\n", buf, feal::get_addr(&recvaddr).c_str(),
                   feal::get_port(&recvaddr).c_str());
    }
}

void Server::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtDgramWriteAvail\n");
}

void Server::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt)
        return;
    printf("Server::EvtSockErr\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    start_listening();
}
