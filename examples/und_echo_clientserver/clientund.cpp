//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "clientund.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MIN(a, b) (a < b ? a : b)
#define SERVERPATH "/tmp/fealundserver"
#define CLIENTPATH "/tmp/fealundclient"

// https://stackoverflow.com/a/323302
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a = a - b;
    a = a - c;
    a = a ^ (c >> 13);
    b = b - c;
    b = b - a;
    b = b ^ (a << 8);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 13);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 12);
    b = b - c;
    b = b - a;
    b = b ^ (a << 16);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 5);
    a = a - b;
    a = a - c;
    a = a ^ (c >> 3);
    b = b - c;
    b = b - a;
    b = b ^ (a << 10);
    c = c - a;
    c = c - b;
    c = c ^ (b >> 15);
    return c;
}

void Clientund::initActor(void)
{
    printf("Clientund::initActor\n");
    timers.init(this);
    dgram.init(this);
    dgram.subscribeReadAvail<EvtDgramReadAvail>();
    dgram.subscribeWriteAvail<EvtDgramWriteAvail>();
    dgram.subscribeSockErr<EvtSockErr>();
    memset(&serveraddr, 0, sizeof(serveraddr));
}

void Clientund::startActor(void)
{
    printf("Clientund::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(15));
    sendToServer();
}

void Clientund::pauseActor(void)
{
    printf("Clientund::pauseActor\n");
}

void Clientund::shutdownActor(void)
{
    printf("Clientund::shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    printf("Client shutdown complete\n");
}

void Clientund::sendToServer(void)
{
    feal::handle_t fd;
    fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    feal::errenum_t se;
    feal::sockaddr_all_t clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));
    serveraddr.un.sun_family = AF_UNIX;
    strcpy(serveraddr.un.sun_path, SERVERPATH);
    clientaddr.un.sun_family = AF_UNIX;
    strcpy(clientaddr.un.sun_path, CLIENTPATH);
    char buf[8];
    srand(mix(clock(), time(nullptr), getpid()));
    snprintf(buf, sizeof(buf), "%d", rand() % 4096);
    strcat(clientaddr.un.sun_path, buf);
    unlink(clientaddr.un.sun_path);
    se = dgram.monitorSock(fd);
    if (se != feal::FEAL_OK)
    {
        printf("Err create sock: %d\n", se);
        return;
    }
    feal::socklen_t length = sizeof(clientaddr.un.sun_family) + strlen(clientaddr.un.sun_path) + 1;
    int ret = bind(fd, &(clientaddr.sa), length);
    if (ret != feal::FEAL_OK)
    {
        se = static_cast<feal::errenum_t>(FEAL_GETHANDLEERRNO);
        printf("Err bind sock: %d\n", se);
        return;
    }
    printf("Unix domain socket datagram client listening on %s ...\n", clientaddr.un.sun_path);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));
}

void Clientund::sendSomething(void)
{
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Client %d", n++);
    printf("Trying to send \"%s\" to %s\n", buf, SERVERPATH);
    feal::socklen_t length = sizeof(serveraddr.un.sun_family) + strlen(serveraddr.un.sun_path) + 1;
    feal::errenum_t se = dgram.sendto((void*)buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes,
                                      &serveraddr, length);
    if (se != feal::FEAL_OK)
        printf("Error sending \"Client n\": %d\n", se);
}

void Clientund::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt)
        return;
    printf("Clientund::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.closeAndReset();
    shutdown();
}

void Clientund::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt)
        return;
    printf("Clientund::EvtDelayTimer\n");
    sendSomething();
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void Clientund::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt)
        return;
    printf("Clientund::EvtDgramReadAvail\n");
    char buf[50];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::sockaddr_all_t recvaddr;
    socklen_t recvaddr_len = sizeof(recvaddr);
    feal::errenum_t se = dgram.recvfrom((void*)buf, sizeof(buf), &bytes, &recvaddr, &recvaddr_len);
    if (se != feal::FEAL_OK)
        printf("Error receiving: %d\n", se);
    else
        printf("Received %lld bytes: \"%s\" from %s\n", (long long int)bytes, buf,
               recvaddr.un.sun_path);
}

void Clientund::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt)
        return;
    printf("Clientund::EvtDgramWriteAvail\n");
    // send_something();
}

void Clientund::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt)
        return;
    printf("Clientund::EvtSockErr\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.closeAndReset();
    sendToServer();
}
