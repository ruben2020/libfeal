//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _CLIENTUND_H
#define _CLIENTUND_H

#include "feal.h"

class EvtEndTimer : public feal::Event
{
public:
EvtEndTimer() = default;
EvtEndTimer( const EvtEndTimer & ) = default;
EvtEndTimer& operator= ( const EvtEndTimer & ) = default;
~EvtEndTimer() = default;
feal::EventId_t getId(void);
};

class EvtDelayTimer : public feal::Event
{
public:
EvtDelayTimer() = default;
EvtDelayTimer( const EvtDelayTimer & ) = default;
EvtDelayTimer& operator= ( const EvtDelayTimer & ) = default;
~EvtDelayTimer() = default;
feal::EventId_t getId(void);
};

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
void handleEvent(std::shared_ptr<feal::EvtDgramReadAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtDgramWriteAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtSockErr> pevt);

protected:

feal::Timers<Clientund> timers;
feal::Datagram<Clientund> dgram;
struct sockaddr_un serveraddr;
virtual void send_to_server(void);

private:
int n=0;

void send_something(void);

};

#endif // _CLIENTUND_H

