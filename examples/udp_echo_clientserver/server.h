#ifndef _SERVER_H
#define _SERVER_H

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

class EvtRetryTimer : public feal::Event
{
public:
EvtRetryTimer() = default;
EvtRetryTimer( const EvtRetryTimer & ) = default;
EvtRetryTimer& operator= ( const EvtRetryTimer & ) = default;
~EvtRetryTimer() = default;
feal::EventId_t getId(void);
};


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
void handleEvent(std::shared_ptr<feal::EvtDgramReadAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtDgramWriteAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtSockErr> pevt);

protected:

feal::Timers<Server> timers;
feal::Datagram<Server> dgram;
virtual void start_listening(void);

private:


};

#endif // _SERVER_H
