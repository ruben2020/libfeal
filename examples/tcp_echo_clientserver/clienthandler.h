/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Licensed under the Apache License, Version 2.0
 * with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You can find a copy of the License in the LICENSE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 * 
 */
 
#ifndef _CLIENTHANDLER_H
#define _CLIENTHANDLER_H

#include "feal.h"

class Server;

class EvtClientDisconnected : public feal::Event
{
friend class ClientHandler;

public:
EvtClientDisconnected() = default;
EvtClientDisconnected( const EvtClientDisconnected & ) = default;
EvtClientDisconnected& operator= ( const EvtClientDisconnected & ) = default;
~EvtClientDisconnected() = default;
feal::EventId_t getId(void);

feal::socket_t client_sockfd = -1;

private:

};


class ClientHandler : public feal::Actor
{

public:

ClientHandler()=default;
~ClientHandler() = default;
void setParam(feal::Stream<Server>* p, feal::socket_t fd, char *s);

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<feal::EvtDataReadAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtDataWriteAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtClientShutdown> pevt);

private:
feal::Stream<Server>* stream = nullptr;
feal::socket_t sockfd = -1;
std::string addrstr;

};


#endif // _CLIENTHANDLER_H

