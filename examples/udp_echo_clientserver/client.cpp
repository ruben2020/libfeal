//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <cstdio>
#include <cstring>
#include "client.h"

#define SERVERPORT 56001
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
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(15));
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
    feal::handle_t fd;
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.in.sin_family = AF_INET;
    serverport = htons(SERVERPORT);
    serveraddr.in.sin_port = serverport;
    feal::inet_pton(AF_INET, "127.0.0.1", &(serveraddr.in.sin_addr));
    feal::errenum se = dgram.monitor_sock(fd);
    if (se != feal::FEAL_OK) printf("Error create sock: %d\n", se);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));
}

void Client::send_something(void)
{
    char buf[50];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    char addrstr[INET6_ADDRSTRLEN];
    feal::inet_ntop(serveraddr.sa.sa_family, &(serveraddr.sa), addrstr, INET6_ADDRSTRLEN);
    snprintf(buf, sizeof(buf), "Client %d", n++);
    printf("Trying to send \"%s\" to %s:%d\n", buf, addrstr, ntohs(serverport));
    feal::errenum se = dgram.sendto((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)),
        &bytes, &serveraddr, sizeof(serveraddr));
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
    char buf[50];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::sockaddr_all recvaddr;
    feal::socklen_t addrsize = sizeof(recvaddr);
    feal::errenum se = dgram.recvfrom((void*) buf, sizeof(buf), &bytes, &recvaddr, &addrsize);
    char addrstr[INET6_ADDRSTRLEN];
    feal::socklen_t addrport;
    feal::inet_ntop(recvaddr.sa.sa_family, &(recvaddr.sa), addrstr, INET6_ADDRSTRLEN);
    addrport = ntohs(recvaddr.sa.sa_family == AF_INET ? recvaddr.in.sin_port : recvaddr.in6.sin6_port);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else printf("Received %lld bytes: \"%s\" from %s:%d\n", (long long int) bytes, buf, addrstr, addrport);
}

void Client::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Client::EvtDgramWriteAvail\n");
    //send_something();
}

void Client::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt) return;
    printf("Client::EvtSockErr\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.close_and_reset();
    send_to_server();
}

