//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "ActorB.h"

#include <cstdio>

#include "ActorA.h"
#include "feal.h"

void EventCabbage::setCabbages(const int& num)
{
    cabbages = num;
}

int EventCabbage::getCabbages(void)
{
    return cabbages;
}

void ActorB::initActor(void)
{
    printf("ActorB initActor\n");
    timers.init(this);
    subscribeEvent<EvtNotifyVege>(this);
    subscribeEvent<EvtNotifyFruit>(this);
}

void ActorB::startActor(void)
{
    printf("ActorB startActor\n");
    timers.startTimer<EventTimerB1>(std::chrono::milliseconds(2000), std::chrono::seconds(2));
    timers.startTimer<EventTimerB2>(std::chrono::seconds(11));
}

void ActorB::pauseActor(void)
{
    printf("ActorB pauseActor\n");
}

void ActorB::shutdownActor(void)
{
    printf("ActorB shutdownActor\n");
}

void ActorB::handleEvent(std::shared_ptr<EvtNotifyVege> pevt)
{
    if (pevt)
        printf("ActorB received carrots = %d, tomatoes = %d\n", pevt.get()->getCarrots(),
               pevt.get()->getTomatoes());
}

void ActorB::handleEvent(std::shared_ptr<EvtNotifyFruit> pevt)
{
    if (pevt)
        printf("ActorB received apples = %d, oranges = %d\n", pevt.get()->getApples(),
               pevt.get()->getOranges());
}

void ActorB::handleEvent(std::shared_ptr<EventTimerB1> pevt)
{
    static int s_cabbages = 0;
    if (!pevt)
        return;
    printf("ActorB::TimerB1 Elapsed - repeats every %lld s\n",
           (long long int)timers.getTimerRepeat<EventTimerB1>().count());
    std::shared_ptr<feal::Event> evt_cabbage = std::make_shared<EventCabbage>();
    ((EventCabbage*)evt_cabbage.get())->setCabbages(s_cabbages++);
    publishEvent(evt_cabbage);
}

void ActorB::handleEvent(std::shared_ptr<EventTimerB2> pevt)
{
    if (!pevt)
        return;
    printf("ActorB::TimerB2 Elapsed\n");
    timers.stopTimer<EventTimerB1>();
}
