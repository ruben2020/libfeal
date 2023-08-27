//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"


class EvtSigInt : public feal::EventSignal
{
public:
EvtSigInt() = default;
EvtSigInt( const EvtSigInt & ) = default;
EvtSigInt& operator= ( const EvtSigInt & ) = default;
~EvtSigInt() = default;
feal::EventId_t getId(void);
};

class EvtSigChld : public feal::EventSignal
{
public:
EvtSigChld() = default;
EvtSigChld( const EvtSigChld & ) = default;
EvtSigChld& operator= ( const EvtSigChld & ) = default;
~EvtSigChld() = default;
feal::EventId_t getId(void);
};

class EvtPipeRead : public feal::EvtReader
{
public:
EvtPipeRead() = default;
EvtPipeRead( const EvtPipeRead & ) = default;
EvtPipeRead& operator= ( const EvtPipeRead & ) = default;
~EvtPipeRead() = default;
feal::EventId_t getId(void);
};

class EvtSockStreamRead : public feal::EvtReader
{
public:
EvtSockStreamRead() = default;
EvtSockStreamRead( const EvtSockStreamRead & ) = default;
EvtSockStreamRead& operator= ( const EvtSockStreamRead & ) = default;
~EvtSockStreamRead() = default;
feal::EventId_t getId(void);
};

class EvtSockDatagramRead : public feal::EvtReader
{
public:
EvtSockDatagramRead() = default;
EvtSockDatagramRead( const EvtSockDatagramRead & ) = default;
EvtSockDatagramRead& operator= ( const EvtSockDatagramRead & ) = default;
~EvtSockDatagramRead() = default;
feal::EventId_t getId(void);
};


class ActorA : public feal::Actor
{

public:

ActorA() = default;
~ActorA() = default;


void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtSigInt> pevt);
void handleEvent(std::shared_ptr<EvtSigChld> pevt);
void handleEvent(std::shared_ptr<EvtPipeRead> pevt);
void handleEvent(std::shared_ptr<EvtSockStreamRead> pevt);
void handleEvent(std::shared_ptr<EvtSockDatagramRead> pevt);

private:
void forkChild(int childnum, const char* medium);

feal::Signal<ActorA> signal;
feal::Reader<ActorA> readerPipe;
feal::Reader<ActorA> readerSockStream;
feal::Reader<ActorA> readerSockDatagram;
std::vector<pid_t> pidvec;

};


#endif // _FEAL_ACTOR_A_H
