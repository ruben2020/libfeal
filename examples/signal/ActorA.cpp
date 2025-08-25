//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "ActorA.h"

#include <cstdio>

#include "feal.h"

void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
    timers.init(this);
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");

    timers.startTimer<EventTimerA1>(std::chrono::seconds(2), std::chrono::seconds(2));
}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EventTimerA1> pevt)
{
    if (!pevt)
        return;
    printf("ActorA::TimerA1 Elapsed - press Ctrl-C to end program\n");
}
