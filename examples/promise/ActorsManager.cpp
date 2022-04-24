/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Licensed under the Apache License, Version 2.0
 * with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You can find a copy of the License in the LICENSE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 * 
 */
 
#include <cstdio>
#include "ActorsManager.h"
#include "ActorA.h"

feal::EventId_t EventTimerShutdown::getId(void)
{
    return getIdOfType<EventTimerShutdown>();
}

feal::EventId_t EvtSigInt::getId(void)
{
    return getIdOfType<EvtSigInt>();
}

feal::EventId_t EvtWorkDone::getId(void)
{
    return getIdOfType<EvtWorkDone>();
}


void ActorsManager::initActor(void)
{
    printf("ActorsManager initActor\n");
    timers.init(this);
    signal.init(this);
    subscribeEvent<EvtWorkDone>(this);
    signal.registersignal<EvtSigInt>(SIGINT);
    actors.push_back(std::make_shared<ActorA>());
    feal::initAll(actors);
}

void ActorsManager::startActor(void)
{
    printf("ActorsManager startActor\n");
    feal::startAll(actors);
    timers.startTimer<EventTimerShutdown>(std::chrono::seconds(180));
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

void ActorsManager::handleEvent(std::shared_ptr<EventTimerShutdown> pevt)
{
    if (!pevt ) return;
    printf("ActorsManager::EventTimerShutdown Elapsed\n");
    shutdown();
}

void ActorsManager::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt ) return;
    printf("ActorsManager::EvtSigInt (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EventTimerShutdown>();
    shutdown();
}

void ActorsManager::handleEvent(std::shared_ptr<EvtWorkDone> pevt)
{
    if (!pevt ) return;
    printf("ActorsManager::EvtWorkDone\n");
    shutdown();
}



