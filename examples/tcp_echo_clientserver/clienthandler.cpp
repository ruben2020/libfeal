/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Dual-licensed under the Apache License, Version 2.0, and
 * the GNU General Public License, Version 2.0;
 * you may not use this file except in compliance
 * with either one of these licenses.
 *
 * You can find copies of these licenses in the included
 * LICENSE-APACHE2 and LICENSE-GPL2 files.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under these licenses is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the license texts for the specific language governing permissions
 * and limitations under the licenses.
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
 *
 */
 
#include <stdio.h>
#include "feal.h"
#include "clienthandler.h"
#include "server.h"

#define MIN(a,b) (a<b ? a : b)

feal::EventId_t EvtClientDisconnected::getId(void)
{
    return getIdOfType<EvtClientDisconnected>();
}


void ClientHandler::setParam(feal::Stream<Server>* p, feal::socket_t fd, char *s)
{
    stream = p;
    sockfd = fd;
    addrstr = s;
}

void ClientHandler::initActor(void)
{
    printf("ClientHandler(%ld)::initActor\n", (long int) sockfd);
}

void ClientHandler::startActor(void)
{
    printf("ClientHandler(%ld)::startActor\n", (long int) sockfd);
    feal::errenum se =feal::FEAL_OK;
    if (stream) se = stream->recv_start(this, sockfd);
    if (se != feal::FEAL_OK)
    {
        printf("Error3 %d\n", se);
    }
}

void ClientHandler::pauseActor(void)
{
    printf("ClientHandler(%ld)::pauseActor\n", (long int) sockfd);
}

void ClientHandler::shutdownActor(void)
{
    printf("ClientHandler(%ld)::shutdownActor\n", (long int) sockfd);
    stream->disconnect_client(sockfd);
}

void ClientHandler::handleEvent(std::shared_ptr<feal::EvtDataReadAvail> pevt)
{
    printf("ClientHandler(%ld)::EvtDataReadAvail\n", (long int) sockfd);
    if ((!pevt)||(!stream)) return;
    char buf[30];
    memset(&buf, 0, sizeof(buf));
    int32_t bytes;
    feal::errenum se = stream->recv((void*) buf, sizeof(buf), &bytes, sockfd);
    if (se != feal::FEAL_OK) printf("Error receiving: %d\n", se);
    else
    {
        printf("Received %d bytes \"%s\" from %s\n", bytes, buf, addrstr.c_str());
        printf("Sending back %d bytes \"%s\" to %s\n", bytes, buf, addrstr.c_str());
        se = stream->send((void*) buf, MIN(strlen(buf) + 1, sizeof(buf)), &bytes, sockfd);
        if (se != feal::FEAL_OK) printf("Error sending \"ClientHandler n\": %d\n", se);
    }
}

void ClientHandler::handleEvent(std::shared_ptr<feal::EvtDataWriteAvail> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%ld)::EvtDataWriteAvail\n", (long int) sockfd);
}
void ClientHandler::handleEvent(std::shared_ptr<feal::EvtClientShutdown> pevt)
{
    if (!pevt) return;
    printf("ClientHandler(%ld)::EvtClientShutdown\n", (long int) sockfd);
    std::shared_ptr<EvtClientDisconnected> pevt2 = std::make_shared<EvtClientDisconnected>();
    pevt2.get()->client_sockfd = sockfd;
    publishEvent(pevt2);
}


