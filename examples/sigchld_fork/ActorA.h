//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtSigInt, EventSignal)
FEAL_EVENT_DEFAULT_DECLARE(EvtSigChld, EventSignal)


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

private:
feal::Signal<ActorA> signal;
std::vector<pid_t> pidvec;

};


#endif // _FEAL_ACTOR_A_H
