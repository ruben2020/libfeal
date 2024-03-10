//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include <cstdio>
#include <cstring>
#include "client.h"

#define MIN(a,b) (a<b ? a : b)


void Client::initActor(void)
{
    printf("Client::initActor\n");
    timers.init(this);
    dgram.init(this);
    dgram.subscribeReadAvail<EvtDgramReadAvail>();
    dgram.subscribeWriteAvail<EvtDgramWriteAvail>();
    dgram.subscribeSockErr<EvtSockErr>();
}

void Client::startActor(void)
{
    printf("Client::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(45));
    send_to_server();
}

void Client::pauseActor(void)
{
    printf("Client::pauseActor\n");
}

void Client::shutdownActor(void)
{
    printf("Client::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("Client shutdown complete\n");
}

void Client::send_to_server(void)
{
    feal::errenum se;
    serveraddr.family = feal::ipaddr::INET;
    serveraddr.port = 12001;
    strcpy(serveraddr.addr, "127.0.0.1");
    se = dgram.create_sock((feal::family_t) serveraddr.family);
    if (se != feal::FEAL_OK) printf("create sock: %d\n", se);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));
}

void Client::send_something(void)
{
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Client %d", n++);
    printf("Trying to send \"%s\" to %s:%d\n", buf, serveraddr.addr, serveraddr.port);
    feal::errenum se = dgram.send_to((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, &serveraddr);
    if (se != feal::FEAL_OK) printf("Error sending \"Client n\": %d\n", se);
}

void Client::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.close_and_reset();
    shutdown();
}

void Client::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDelayTimer\n");
    send_something();
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void Client::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDgramReadAvail\n");
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::ipaddr recvaddr;
    feal::errenum se = dgram.recv_from((void*) buf, sizeof(buf), &bytes, &recvaddr);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else printf("Received %d bytes: \"%s\" from %s:%d\n", bytes, buf, recvaddr.addr, recvaddr.port);
}

void Client::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDgramWriteAvail\n");
    send_something();
}

void Client::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt) return;
    printf("Client::EvtSockErr\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.close_and_reset();
    send_to_server();
}

