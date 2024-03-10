//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"

class EventCabbage;

class EvtNotifyVege : public feal::Event
{
friend class ActorA;

public:
EvtNotifyVege() = default;
EvtNotifyVege( const EvtNotifyVege & ) = default;
EvtNotifyVege& operator= ( const EvtNotifyVege & ) = default;
~EvtNotifyVege() = default;
FEAL_EVENT_GETID(EvtNotifyVege)

int get_carrots(void);
int get_tomatoes(void);

private:
void set_carrots(const int& num);
void set_tomatoes(const int& num);
int carrots=0;
int tomatoes=0;

};

class EvtNotifyFruit : public feal::Event
{
friend class ActorA;

public:
EvtNotifyFruit() = default;
EvtNotifyFruit( const EvtNotifyFruit & ) = default;
EvtNotifyFruit& operator= ( const EvtNotifyFruit & ) = default;
~EvtNotifyFruit() = default;
FEAL_EVENT_GETID(EvtNotifyFruit)

int get_apples(void);
int get_oranges(void);

private:
void set_apples(const int& num);
void set_oranges(const int& num);
int apples=0;
int oranges=0;

};

FEAL_EVENT_DEFAULT_DECLARE(EventTimerA1, Event)

class ActorA : public feal::Actor
{

public:

ActorA() = default;
~ActorA() = default;


void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EventCabbage> pevt);
void handleEvent(std::shared_ptr<EventTimerA1> pevt);

private:
feal::Timers<ActorA> timers;

};


#endif // _FEAL_ACTOR_A_H
