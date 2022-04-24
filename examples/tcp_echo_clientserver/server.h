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
 
#ifndef _SERVER_H
#define _SERVER_H

#include "feal.h"

class ClientHandler;
class EvtClientDisconnected;

class EvtEndTimer : public feal::Event
{
public:
EvtEndTimer() = default;
EvtEndTimer( const EvtEndTimer & ) = default;
EvtEndTimer& operator= ( const EvtEndTimer & ) = default;
~EvtEndTimer() = default;
feal::EventId_t getId(void);
};

class EvtRetryTimer : public feal::Event
{
public:
EvtRetryTimer() = default;
EvtRetryTimer( const EvtRetryTimer & ) = default;
EvtRetryTimer& operator= ( const EvtRetryTimer & ) = default;
~EvtRetryTimer() = default;
feal::EventId_t getId(void);
};

class EvtSigInt : public feal::EventSignal
{
public:
EvtSigInt() = default;
EvtSigInt( const EvtSigInt & ) = default;
EvtSigInt& operator= ( const EvtSigInt & ) = default;
~EvtSigInt() = default;
feal::EventId_t getId(void);
};


class Server : public feal::Actor
{

public:

Server() = default;
~Server() = default;


virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
void handleEvent(std::shared_ptr<feal::EvtIncomingConn> pevt);
void handleEvent(std::shared_ptr<feal::EvtServerShutdown> pevt);
void handleEvent(std::shared_ptr<EvtClientDisconnected> pevt);
void handleEvent(std::shared_ptr<EvtSigInt> pevt);

protected:

feal::Timers<Server> timers;
feal::Stream<Server> stream;
feal::Signal<Server> signal;
virtual void start_server(void);
virtual void print_client_address(feal::socket_t fd);
virtual void get_client_address(feal::socket_t fd, char* addr);

private:
std::map<feal::socket_t, std::shared_ptr<ClientHandler>> mapch;

};

#endif // _SERVER_H
