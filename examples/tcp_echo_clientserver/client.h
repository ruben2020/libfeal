//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _CLIENT_H
#define _CLIENT_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtRetryTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtSigInt, EventSignal)
FEAL_EVENT_DEFAULT_DECLARE(EvtConnectedToServer, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDataReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDataWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtConnectionShutdown, EventComm)


class Client : public feal::Actor
{

public:

Client() = default;
~Client() = default;

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
void handleEvent(std::shared_ptr<EvtDelayTimer> pevt);
void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
void handleEvent(std::shared_ptr<EvtConnectedToServer> pevt);
void handleEvent(std::shared_ptr<EvtDataReadAvail> pevt);
void handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt);
void handleEvent(std::shared_ptr<EvtConnectionShutdown> pevt);
void handleEvent(std::shared_ptr<EvtSigInt> pevt);

protected:

feal::Timers<Client> timers;
feal::Stream<Client> stream;
feal::Signal<Client> signal;
virtual void connect_to_server(void);

private:
int n=0;

void send_something(void);

};

#endif // _CLIENT_H

