//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _CLIENTUND_H
#define _CLIENTUND_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtSockErr, EventComm)


class Clientund : public feal::Actor
{

public:

Clientund() = default;
~Clientund() = default;

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

feal::Timers<Clientund> timers;
feal::Datagram<Clientund> dgram;
feal::sockaddr_all serveraddr;
virtual void send_to_server(void);

private:
int n=0;

void send_something(void);

};

#endif // _CLIENTUND_H

