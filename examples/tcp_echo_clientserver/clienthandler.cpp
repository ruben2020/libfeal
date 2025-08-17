//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <stdio.h>
#include "feal.h"
#include "clienthandler.h"
#include "server.h"

#define MIN(a,b) (a<b ? a : b)


void ClientHandler::setParam(feal::Stream<Server>* p, feal::handle_t fd, char *s)
{
    stream = p;
    sockfd = fd;
    addrstr = s;
}

void ClientHandler::initActor(void)
{
    printf("ClientHandler(%lld)::initActor\n", (long long int) sockfd);
}

void ClientHandler::startActor(void)
{
    printf("ClientHandler(%lld)::startActor\n", (long long int) sockfd);
    feal::errenum se =feal::FEAL_OK;
    if (stream) se = stream->registerClient<ClientHandler, EvtDataReadAvail, EvtDataWriteAvail, EvtClientShutdown>(this, sockfd);
    if (se != feal::FEAL_OK)
    {
        printf("Error3 %d\n", se);
    }
}

void ClientHandler::pauseActor(void)
{
    printf("ClientHandler(%lld)::pauseActor\n", (long long int) sockfd);
}

void ClientHandler::shutdownActor(void)
{
    printf("ClientHandler(%lld)::shutdownActor\n", (long long int) sockfd);
    stream->disconnect_client(sockfd);
}

void ClientHandler::handleEvent(std::shared_ptr<EvtDataReadAvail> pevt)
{
    printf("ClientHandler(%lld)::EvtDataReadAvail\n", (long long int) sockfd);
    if ((!pevt)||(!stream)) return;
    char buf[30];
    memset(&buf, 0, sizeof(buf));
    int32_t bytes;
    feal::errenum se = stream->recv((void*) buf, sizeof(buf), &bytes, sockfd);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else
    {
        printf("Received %lld bytes \"%s\" from %s\n", (long long int) bytes, buf, addrstr.c_str());
        printf("Sending back %lld bytes \"%s\" to %s\n", (long long int) bytes, buf, addrstr.c_str());
        se = stream->send((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, sockfd);
        if (se != feal::FEAL_OK) printf("Error sending \"ClientHandler n\": %d\n", se);
    }
}

void ClientHandler::handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%lld)::EvtDataWriteAvail\n", (long long int) sockfd);
}

void ClientHandler::handleEvent(std::shared_ptr<EvtClientShutdown> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%lld)::EvtClientShutdown\n", (long long int) sockfd);
    std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
    pevt2.get()->fd = sockfd;
    publishEvent(pevt2);
}


