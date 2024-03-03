//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
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

feal::handle_t fd = -1;

private:

};

class EvtDataReadAvail : public feal::EventComm
{
public:
EvtDataReadAvail() = default;
EvtDataReadAvail( const EvtDataReadAvail & ) = default;
EvtDataReadAvail& operator= ( const EvtDataReadAvail & ) = default;
~EvtDataReadAvail() = default;
feal::EventId_t getId(void);
};

class EvtDataWriteAvail : public feal::EventComm
{
public:
EvtDataWriteAvail() = default;
EvtDataWriteAvail( const EvtDataWriteAvail & ) = default;
EvtDataWriteAvail& operator= ( const EvtDataWriteAvail & ) = default;
~EvtDataWriteAvail() = default;
feal::EventId_t getId(void);
};

class EvtClientShutdown : public feal::EventComm
{
public:
EvtClientShutdown() = default;
EvtClientShutdown( const EvtClientShutdown & ) = default;
EvtClientShutdown& operator= ( const EvtClientShutdown & ) = default;
~EvtClientShutdown() = default;
feal::EventId_t getId(void);
};



class ClientHandler : public feal::Actor
{

public:

ClientHandler()=default;
~ClientHandler() = default;
void setParam(feal::Stream<Server>* p, feal::handle_t fd, char *s);

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtDataReadAvail> pevt);
void handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt);
void handleEvent(std::shared_ptr<EvtClientShutdown> pevt);

private:
feal::Stream<Server>* stream = nullptr;
feal::handle_t sockfd = -1;
std::string addrstr;

};


#endif // _CLIENTHANDLER_H

