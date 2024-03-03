//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
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

class EvtSigInt : public feal::EventSignal
{
public:
EvtSigInt() = default;
EvtSigInt( const EvtSigInt & ) = default;
EvtSigInt& operator= ( const EvtSigInt & ) = default;
~EvtSigInt() = default;
feal::EventId_t getId(void);
};

class EvtConnectedToServer : public feal::EventComm
{
public:
EvtConnectedToServer() = default;
EvtConnectedToServer( const EvtConnectedToServer & ) = default;
EvtConnectedToServer& operator= ( const EvtConnectedToServer & ) = default;
~EvtConnectedToServer() = default;
feal::EventId_t getId(void);
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

class EvtConnectionShutdown : public feal::EventComm
{
public:
EvtConnectionShutdown() = default;
EvtConnectionShutdown( const EvtConnectionShutdown & ) = default;
EvtConnectionShutdown& operator= ( const EvtConnectionShutdown & ) = default;
~EvtConnectionShutdown() = default;
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

