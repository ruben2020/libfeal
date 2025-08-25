//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "client.h"

#include <cstdio>
#include <cstring>

#define SERVERPORT 56001
#define MIN(a, b) (a < b ? a : b)

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
    if (se != feal::FEAL_OK)
        printf("Error create sock: %d\n", se);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));
}

void Client::send_something(void)
{
    char buf[50];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Client %d", n++);
    printf("Trying to send \"%s\" to %s:%s\n", buf, feal::get_addr(&serveraddr).c_str(),
           feal::get_port(&serveraddr).c_str());
    feal::errenum se = dgram.sendto((void*)buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes,
                                    &serveraddr, sizeof(serveraddr));
    if (se != feal::FEAL_OK)
        printf("Error sending \"Client n\": %d\n", se);
}

void Client::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.close_and_reset();
    shutdown();
}

void Client::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtDelayTimer\n");
    send_something();
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void Client::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtDgramReadAvail\n");
    char buf[50];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::sockaddr_all recvaddr;
    feal::socklen_t addrsize = sizeof(recvaddr);
    feal::errenum se = dgram.recvfrom((void*)buf, sizeof(buf), &bytes, &recvaddr, &addrsize);
    if (se != feal::FEAL_OK)
        printf("Error receiving: %d\n", se);
    else
        printf("Received %lld bytes: \"%s\" from %s:%s\n", (long long int)bytes, buf,
               feal::get_addr(&recvaddr).c_str(), feal::get_port(&recvaddr).c_str());
}

void Client::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtDgramWriteAvail\n");
    // send_something();
}

void Client::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtSockErr\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.close_and_reset();
    send_to_server();
}
