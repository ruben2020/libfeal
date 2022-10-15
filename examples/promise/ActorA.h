//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"


class EvtPromiseComplete : public feal::Event
{
public:
EvtPromiseComplete() = default;
EvtPromiseComplete( const EvtPromiseComplete & ) = default;
EvtPromiseComplete& operator= ( const EvtPromiseComplete & ) = default;
~EvtPromiseComplete() = default;
feal::EventId_t getId(void);
int total = 0;
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

void handleEvent(std::shared_ptr<EvtPromiseComplete> pevt);

private:
feal::Timers<ActorA> timers;
std::thread work_thread;
std::shared_future<std::shared_ptr<EvtPromiseComplete>> fut_acc;

static void accThreadLauncher(ActorA* ptr, std::vector<int> vec, 
    std::promise<std::shared_ptr<EvtPromiseComplete>> prom);
void accumulator(std::vector<int> vec,
    std::promise<std::shared_ptr<EvtPromiseComplete>> prom);

};


#endif // _FEAL_ACTOR_A_H
