#ifndef _CLIENT_H
#define _CLIENT_H

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

class EvtRetryTimer : public feal::Event
{
public:
EvtRetryTimer() = default;
EvtRetryTimer( const EvtRetryTimer & ) = default;
EvtRetryTimer& operator= ( const EvtRetryTimer & ) = default;
~EvtRetryTimer() = default;
feal::EventId_t getId(void);
};


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
void handleEvent(std::shared_ptr<feal::EvtConnectedToServer> pevt);
void handleEvent(std::shared_ptr<feal::EvtDataReadAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtDataWriteAvail> pevt);
void handleEvent(std::shared_ptr<feal::EvtConnectionShutdown> pevt);

protected:

feal::Timers<Client> timers;
feal::Stream<Client> stream;
virtual void connect_to_server(void);

private:
int n=0;

void send_something(void);

};

#endif // _CLIENT_H

