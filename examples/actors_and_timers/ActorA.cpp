//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "ActorA.h"

#include <stdio.h>

#include "ActorB.h"
#include "feal.h"

int EvtNotifyVege::get_carrots(void)
{
    return carrots;
}

int EvtNotifyVege::get_tomatoes(void)
{
    return tomatoes;
}
void EvtNotifyVege::set_carrots(const int& num)
{
    carrots = num;
}
void EvtNotifyVege::set_tomatoes(const int& num)
{
    tomatoes = num;
}

int EvtNotifyFruit::get_apples(void)
{
    return apples;
}

int EvtNotifyFruit::get_oranges(void)
{
    return oranges;
}
void EvtNotifyFruit::set_apples(const int& num)
{
    apples = num;
}
void EvtNotifyFruit::set_oranges(const int& num)
{
    oranges = num;
}

void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
    timers.init(this);
    subscribeEvent<EventCabbage>(this);
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");

    timers.startTimer<EventTimerA1>(std::chrono::seconds(3), std::chrono::seconds(3));

    std::shared_ptr<feal::Event> pevt = std::make_shared<EvtNotifyVege>();
    ((EvtNotifyVege*)pevt.get())->set_carrots(10);
    ((EvtNotifyVege*)pevt.get())->set_tomatoes(20);
    publishEvent(pevt);

    std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtNotifyFruit>();
    ((EvtNotifyFruit*)pevt2.get())->set_apples(30);
    ((EvtNotifyFruit*)pevt2.get())->set_oranges(40);
    publishEvent(pevt2);
}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EventCabbage> pevt)
{
    if (!pevt)
        return;
    printf("ActorA received cabbages = %d\n", pevt.get()->get_cabbages());

    std::shared_ptr<feal::Event> pevt1 = std::make_shared<EvtNotifyVege>();
    ((EvtNotifyVege*)pevt1.get())->set_carrots(100);
    ((EvtNotifyVege*)pevt1.get())->set_tomatoes(200);
    publishEvent(pevt1);

    std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtNotifyFruit>();
    ((EvtNotifyFruit*)pevt2.get())->set_apples(300);
    ((EvtNotifyFruit*)pevt2.get())->set_oranges(400);
    publishEvent(pevt2);

    timers.setTimerRepeat<EventTimerA1>(std::chrono::seconds(1));
}

void ActorA::handleEvent(std::shared_ptr<EventTimerA1> pevt)
{
    if (!pevt)
        return;
    printf("ActorA::TimerA1 Elapsed\n");
}
