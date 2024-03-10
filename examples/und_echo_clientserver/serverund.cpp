//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include <cstdio>
#include <cstring>
#include "serverund.h"

#define MIN(a,b) (a<b ? a : b)
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
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(80));
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
    struct sockaddr_un serveraddr;
    feal::errenum se;
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SERVERPATH);
    unlink(SERVERPATH);
    se = dgram.create_sock((feal::family_t) serveraddr.sun_family);
    if (se != feal::FEAL_OK)
    {
        printf("create sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    se = dgram.bind_sock(&serveraddr);
    if (se != feal::FEAL_OK)
    {
        printf("bind sock: %d\n", se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
        return;
    }
    printf("Unix domain socket datagram server listening on %s ...\n", SERVERPATH);
}


void Serverund::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Serverund::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    shutdown();
}

void Serverund::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt) return;
    printf("Serverund::EvtRetryTimer\n");
    start_listening();
}

void Serverund::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt) return;
    printf("Serverund::EvtDgramReadAvail\n");
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    struct sockaddr_un recvaddr;
    socklen_t recvaddr_len = sizeof(recvaddr);
    feal::errenum se = dgram.recv_from((void*) buf, sizeof(buf), &bytes, &recvaddr, &recvaddr_len);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else printf("Received %d bytes: \"%s\" from %s\n", bytes, buf, recvaddr.sun_path);
    printf("Sending back \"%s\" to %s\n", buf, recvaddr.sun_path);
    se = dgram.send_to((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, &recvaddr, recvaddr_len);
    if (se != feal::FEAL_OK) printf("Error sending back \"%s\" to %s\n", buf, recvaddr.sun_path);
}

void Serverund::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Serverund::EvtDgramWriteAvail\n");
}

void Serverund::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt) return;
    printf("Serverund::EvtSockErr\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    start_listening();
}
