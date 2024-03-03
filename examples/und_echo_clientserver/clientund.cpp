//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "clientund.h"

#define MIN(a,b) (a<b ? a : b)
#define SERVERPATH "/tmp/fealundserver"
#define CLIENTPATH "/tmp/fealundclient"

// https://stackoverflow.com/a/323302
unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
{
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

feal::EventId_t EvtEndTimer::getId(void)
{
    return getIdOfType<EvtEndTimer>();
}

feal::EventId_t EvtDelayTimer::getId(void)
{
    return getIdOfType<EvtDelayTimer>();
}

feal::EventId_t EvtDgramReadAvail::getId(void)
{
    return getIdOfType<EvtDgramReadAvail>();
}

feal::EventId_t EvtDgramWriteAvail::getId(void)
{
    return getIdOfType<EvtDgramWriteAvail>();
}

feal::EventId_t EvtSockErr::getId(void)
{
    return getIdOfType<EvtSockErr>();
}


void Clientund::initActor(void)
{
    printf("Clientund::initActor\n");
    timers.init(this);
    dgram.init(this);
    dgram.subscribeReadAvail<EvtDgramReadAvail>();
    dgram.subscribeWriteAvail<EvtDgramWriteAvail>();
    dgram.subscribeSockErr<EvtSockErr>();
}

void Clientund::startActor(void)
{
    printf("Clientund::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(45));
    send_to_server();
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

void Clientund::send_to_server(void)
{
    feal::errenum se;
    struct sockaddr_un clientaddr;
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SERVERPATH);
    clientaddr.sun_family = AF_UNIX;
    strcpy(clientaddr.sun_path, CLIENTPATH);
    char buf[8];
    srand(mix(clock(), time(NULL), getpid()));
    sprintf(buf, "%d", rand() % 4096);
    strcat(clientaddr.sun_path, buf);
    unlink(clientaddr.sun_path);
    se = dgram.create_sock((feal::family_t) clientaddr.sun_family);
    if (se != feal::FEAL_OK)
    {
        printf("create sock: %d\n", se);
        return;
    }
    se = dgram.bind_sock(&clientaddr);
    if (se != feal::FEAL_OK)
    {
        printf("bind sock: %d\n", se);
        return;
    }
    printf("Unix domain socket datagram client listening on %s ...\n", clientaddr.sun_path);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(1));
}

void Clientund::send_something(void)
{
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    sprintf(buf, "Client %d", n++);
    printf("Trying to send \"%s\" to %s\n", buf, SERVERPATH);
    feal::errenum se = dgram.send_to((void*) buf, MIN(strlen(buf) + 1,
        sizeof(buf)), &bytes, &serveraddr, sizeof(serveraddr));
    if (se != feal::FEAL_OK) printf("Error sending \"Client n\": %d\n", se);
}

void Clientund::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt) return;
    printf("Clientund::EvtEndTimer Elapsed\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.close_and_reset();
    shutdown();
}

void Clientund::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt) return;
    printf("Clientund::EvtDelayTimer\n");
    send_something();
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void Clientund::handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt)
{
    if (!pevt) return;
    printf("Clientund::EvtDgramReadAvail\n");
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    struct sockaddr_un recvaddr;
    socklen_t recvaddr_len = sizeof(recvaddr);
    feal::errenum se = dgram.recv_from((void*) buf,
        sizeof(buf), &bytes, &recvaddr, &recvaddr_len);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else printf("Received %d bytes: \"%s\" from %s\n", bytes, buf, recvaddr.sun_path);
}

void Clientund::handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt)
{
    if (!pevt) return;
    printf("Clientund::EvtDgramWriteAvail\n");
    send_something();
}

void Clientund::handleEvent(std::shared_ptr<EvtSockErr> pevt)
{
    if (!pevt) return;
    printf("Clientund::EvtSockErr\n");
    timers.stopTimer<EvtDelayTimer>();
    dgram.close_and_reset();
    send_to_server();
}

