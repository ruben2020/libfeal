//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FEAL_ACTORSMANAGER_H
#define _FEAL_ACTORSMANAGER_H

#include <memory>
#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EventTimerShutdown, Event)

class ActorsManager : public feal::Actor
{

public:

ActorsManager() = default;
~ActorsManager() = default;


virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);

void handleEvent(std::shared_ptr<EventTimerShutdown> pevt);

private:
std::vector<std::shared_ptr<feal::Actor>> actors;
feal::Timers<ActorsManager> timers;

};


#endif // _FEAL_ACTORSMANAGER_H
