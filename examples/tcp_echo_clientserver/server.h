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
void handleEvent(std::shared_ptr<feal::EvtIncomingConn> pevt);
void handleEvent(std::shared_ptr<feal::EvtServerShutdown> pevt);
void handleEvent(std::shared_ptr<EvtClientDisconnected> pevt);

protected:

virtual void start_server(void);
virtual void print_client_address(feal::socket_t fd);

private:
feal::Timers<Server> timers;
feal::Stream<Server> stream;
std::map<feal::socket_t, std::shared_ptr<ClientHandler>> mapch;

};

#endif // _SERVER_H
