//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "server.h"

#define MIN(a,b) (a<b ? a : b)


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
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(80));
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
    feal::ipaddr serveraddr;
    feal::errenum se;
    serveraddr.family = feal::ipaddr::INET;
    serveraddr.port = 12001;
    strcpy(serveraddr.addr, "127.0.0.1");
    se = dgram.create_sock((feal::family_t) serveraddr.family);
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
    printf("UDP Listening on %s:%d ...\n", serveraddr.addr, serveraddr.port );
}


void Server::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Server::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    shutdown();
}

void Server::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt) return;
    printf("Server::EvtRetryTimer\n");
    start_listening();
}

void Server::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt) return;
    printf("Server::EvtDgramReadAvail\n");
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::ipaddr recvaddr;
    feal::errenum se = dgram.recv_from((void*) buf, sizeof(buf), &bytes, &recvaddr);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else printf("Received %d bytes: \"%s\" from %s:%d\n", bytes, buf, recvaddr.addr, recvaddr.port);
    printf("Sending back \"%s\" to %s:%d\n", buf, recvaddr.addr, recvaddr.port);
    se = dgram.send_to((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, &recvaddr);
    if (se != feal::FEAL_OK) printf("Error sending back \"%s\" to %s:%d\n", buf, recvaddr.addr, recvaddr.port);
}

void Server::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Server::EvtDgramWriteAvail\n");
}

void Server::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt) return;
    printf("Server::EvtSockErr\n");
    timers.stopTimer<EvtRetryTimer>();
    dgram.close_and_reset();
    start_listening();
}
