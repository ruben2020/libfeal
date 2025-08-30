//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "client.h"

#include <cstdio>
#include <cstring>

#define SERVERPORT 55001
#define MIN(a, b) (a < b ? a : b)

void Client::initActor(void)
{
    printf("Client::initActor\n");
    timers.init(this);
    stream.init(this);
    stream.subscribeConnectedToServer<EvtConnectedToServer>();
    stream.subscribeReadAvail<EvtDataReadAvail>();
    stream.subscribeWriteAvail<EvtDataWriteAvail>();
    stream.subscribeConnectionShutdown<EvtConnectionShutdown>();
    signal.init(this);
    signal.registerSignal<EvtSigInt>(SIGINT);
}

void Client::startActor(void)
{
    printf("Client::startActor\n");
    timers.startTimer<EvtEndTimer>(std::chrono::seconds(15));
    connectToServer();
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

void Client::connectToServer(void)
{
    feal::handle_t fd;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    feal::sockaddr_all_t sall;
    memset(&sall, 0, sizeof(sall));
    sall.in.sin_family = AF_INET;
    sall.in.sin_port = htons(SERVERPORT);
    inet_pton(AF_INET, "127.0.0.1", &(sall.in.sin_addr));
    printf("Trying to connect to 127.0.0.1:%d\n", SERVERPORT);
    feal::errenum_t se = stream.connect(fd, &sall, sizeof(sall));
    if (se != feal::FEAL_OK)
    {
        printf("Error connecting to 127.0.0.1:%d  err %d\n", SERVERPORT, se);
        timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
    }
}

void Client::sendSomething(void)
{
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Client %d", n++);
    printf("Trying to send \"%s\"\n", buf);
    feal::errenum_t se = stream.send((void*)buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes);
    if (se != feal::FEAL_OK)
        printf("Error sending \"Client n\": %d\n", se);
}

void Client::handleEvent(std::shared_ptr<EvtEndTimer> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtEndTimer Elapsed\n");
    stream.disconnectAndReset();
    shutdown();
}

void Client::handleEvent(std::shared_ptr<EvtDelayTimer> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtDelayTimer\n");
    sendSomething();
}

void Client::handleEvent(std::shared_ptr<EvtRetryTimer> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtRetryTimer\n");
    connectToServer();
}

void Client::handleEvent(std::shared_ptr<EvtConnectedToServer> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtConnectedToServer\n");
    char buf[30];
    strcpy(buf, "Hello! Client here!");
    int32_t bytes;
    printf("Trying to send \"Hello! Client here!\"\n");
    feal::errenum_t se = stream.send((void*)buf, sizeof(buf), &bytes);
    if (se != feal::FEAL_OK)
        printf("Error sending \"Hello! Client here!\": %d\n", se);
}

void Client::handleEvent(std::shared_ptr<EvtDataReadAvail> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtDataReadAvail\n");
    char buf[30];
    int32_t bytes;
    memset(&buf, 0, sizeof(buf));
    feal::errenum_t se = stream.recv((void*)buf, sizeof(buf), &bytes);
    if (se != feal::FEAL_OK)
        printf("Error receiving: %d\n", se);
    else
        printf("Received %lld bytes: \"%s\"\n", (long long int)bytes, buf);
    timers.startTimer<EvtDelayTimer>(std::chrono::seconds(2));
}

void Client::handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtDataWriteAvail\n");
    // send_something();
}

void Client::handleEvent(std::shared_ptr<EvtConnectionShutdown> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtConnectionShutdown\n");
    timers.stopTimer<EvtDelayTimer>();
    stream.disconnectAndReset();
    timers.startTimer<EvtRetryTimer>(std::chrono::seconds(5));
}

void Client::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt)
        return;
    printf("Client::EvtSigInt (signum=%d, sicode=%d)\n", pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EvtEndTimer>();
    stream.disconnectAndReset();
    shutdown();
}
