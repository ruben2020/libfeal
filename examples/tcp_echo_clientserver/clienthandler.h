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
void setParam(feal::Stream<Server>* p, feal::socket_t fd);

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

};


#endif // _CLIENTHANDLER_H

