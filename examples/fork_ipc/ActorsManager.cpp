//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <cstdio>
#include "ActorsManager.h"
#include "ActorA.h"

feal::EventId_t EvtAllDone::getId(void)
{
    return getIdOfType<EvtAllDone>();
}

void ActorsManager::initActor(void)
{
    printf("ActorsManager initActor\n");
    subscribeEvent<EvtAllDone>(this);
    actors.push_back(std::make_shared<ActorA>());
    feal::initAll(actors);
}

void ActorsManager::startActor(void)
{
    printf("ActorsManager startActor\n");
    feal::startAll(actors);
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
    printf("Signal shutdown complete\n");
}

void ActorsManager::handleEvent(std::shared_ptr<EvtAllDone> pevt)
{
    if (!pevt ) return;
    printf("ActorsManager::EvtAllDone\n");
    shutdown();
}
