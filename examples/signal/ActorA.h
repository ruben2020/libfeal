//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_ACTOR_A_H
#define FEAL_ACTOR_A_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EventTimerA1, Event)

class ActorA : public feal::Actor
{
   public:
    ActorA() = default;
    ~ActorA() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EventTimerA1> pevt);

   private:
    feal::Timers<ActorA> timers;
};

#endif  // FEAL_ACTOR_A_H
