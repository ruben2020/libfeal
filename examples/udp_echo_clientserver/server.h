//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _SERVER_H
#define _SERVER_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtRetryTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtSockErr, EventComm)


class Server : public feal::Actor
{

public:

Server() = default;
~Server() = default;

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
void handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt);
void handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt);
void handleEvent(std::shared_ptr<EvtSockErr> pevt);

protected:

feal::Timers<Server> timers;
feal::Datagram<Server> dgram;
virtual void start_listening(void);

private:


};

#endif // _SERVER_H
