//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _FIFOREADER_H
#define _FIFOREADER_H

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

class EvtFifoRead : public feal::EvtReader
{
public:
EvtFifoRead() = default;
EvtFifoRead( const EvtFifoRead & ) = default;
EvtFifoRead& operator= ( const EvtFifoRead & ) = default;
~EvtFifoRead() = default;
feal::EventId_t getId(void);
};


class Fiforeader : public feal::Actor
{

public:

Fiforeader() = default;
~Fiforeader() = default;

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
void handleEvent(std::shared_ptr<EvtFifoRead> pevt);

private:
void open_for_reading(void);
feal::Timers<Fiforeader> timers;
feal::FifoReader<Fiforeader> reader;

};

#endif // _FIFOREADER_H
