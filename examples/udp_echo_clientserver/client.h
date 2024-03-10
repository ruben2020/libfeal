//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _CLIENT_H
#define _CLIENT_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtSockErr, EventComm)


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
void handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt);
void handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt);
void handleEvent(std::shared_ptr<EvtSockErr> pevt);

protected:

feal::Timers<Client> timers;
feal::Datagram<Client> dgram;
feal::ipaddr serveraddr;
virtual void send_to_server(void);

private:
int n=0;

void send_something(void);

};

#endif // _CLIENT_H

