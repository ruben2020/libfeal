//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "serverund.h"

#include <cstdio>
#include <cstring>

#define MIN(a, b) (a < b ? a : b)
#define SERVERPATH "/tmp/fealundserver"

void Serverund::initActor(void)
{
    printf("Serverund::initActor\n");
    timers.init(this);
    dgram.init(this);
    dgram.subscribeReadAvail<EvtDgramReadAvail>();
    dgram.subscribeWriteAvail<EvtDgramWriteAvail>();
    dgram.subscribeSockErr<EvtSockErr>();
}

void Serverund::startActor(void)
{
    printf("Serverund::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(15));
    start_listening();
}

void Serverund::pauseActor(void)
{
    printf("Serverund::pauseActor\n");
}

void Serverund::shutdownActor(void)
{
    printf("Serverund::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("Client shutdown complete\n");
}

void Serverund::start_listening(void)
{
    feal::handle_t fd;
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    feal::sockaddr_all serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    feal::errenum se;
    serveraddr.un.sun_family = AF_UNIX;
    strcpy(serveraddr.un.sun_path, SERVERPATH);
    unlink(SERVERPATH);
    se = dgram.monitor_sock(fd);
    if (se != feal::FEAL_OK)
    {
        printf("create sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    feal::socklen_t length = sizeof(serveraddr.un.sun_family) + strlen(serveraddr.un.sun_path) + 1;
    int ret = bind(fd, &(serveraddr.sa), length);
    if (ret != feal::FEAL_OK)
    {
        se = static_cast<feal::errenum>(FEAL_GETHANDLEERRNO);
        printf("bind sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Unix domain socket datagram server listening on %s ...\n", SERVERPATH);
}

void Serverund::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt)
        return;
    printf("Serverund::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    shutdown();
}

void Serverund::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt)
        return;
    printf("Serverund::EvtRetryTimer\n");
    start_listening();
}

void Serverund::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt)
        return;
    printf("Serverund::EvtDgramReadAvail\n");
    char buf[50];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::sockaddr_all recvaddr;
    memset(&recvaddr, 0, sizeof(recvaddr));
    socklen_t recvaddr_len;
    feal::errenum se = dgram.recvfrom((void*)buf, sizeof(buf), &bytes, &recvaddr, &recvaddr_len);
    if (se != feal::FEAL_OK)
        printf("Error receiving: %d\n", se);
    else
        printf("Received %lld bytes: \"%s\" from %s\n", (long long int)bytes, buf,
               recvaddr.un.sun_path);
    printf("Sending back \"%s\" to %s\n", buf, recvaddr.un.sun_path);
    recvaddr_len = sizeof(recvaddr.un.sun_family) + strlen(recvaddr.un.sun_path) + 1;
    se = dgram.sendto((void*)buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, &recvaddr,
                      recvaddr_len);
    if (se != feal::FEAL_OK)
        printf("Error sending back \"%s\" to %s\n", buf, recvaddr.un.sun_path);
}

void Serverund::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt)
        return;
    printf("Serverund::EvtDgramWriteAvail\n");
}

void Serverund::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt)
        return;
    printf("Serverund::EvtSockErr\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    start_listening();
}
