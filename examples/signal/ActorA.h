//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"


class EventTimerA1 : public feal::Event
{
public:
EventTimerA1() = default;
EventTimerA1( const EventTimerA1 & ) = default;
EventTimerA1& operator= ( const EventTimerA1 & ) = default;
~EventTimerA1() = default;
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

void handleEvent(std::shared_ptr<EventTimerA1> pevt);

private:
feal::Timers<ActorA> timers;

};


#endif // _FEAL_ACTOR_A_H
