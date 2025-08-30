//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "ActorsManager.h"

#include <cstdio>

#include "ActorA.h"
#include "ActorB.h"

void ActorsManager::initActor(void)
{
    printf("ActorsManager initActor\n");
    timers.init(this);
    actors.push_back(std::make_shared<ActorA>());
    actors.push_back(std::make_shared<ActorB>());
    feal::initAll(actors);
}

void ActorsManager::startActor(void)
{
    printf("ActorsManager startActor\n");
    feal::startAll(actors);
    timers.startTimer<EventTimerShutdown>(std::chrono::seconds(11));
}

void ActorsManager::pauseActor(void)
{
    printf("ActorsManager pauseActor\n");
    feal::pauseAll(actors);
}

void ActorsManager::shutdownActor(void)
{
    printf("ActorsManager shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    feal::shutdownAll(actors);
    actors.clear();
    printf("Actors&Timers shutdown complete\n");
}

void ActorsManager::handleEvent(std::shared_ptr<EventTimerShutdown> pevt)
{
    if (!pevt)
        return;
    printf("ActorsManager::EventTimerShutdown Elapsed\n");
    shutdown();
}
