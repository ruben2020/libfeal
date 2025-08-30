//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_ACTORSMANAGER_H
#define FEAL_ACTORSMANAGER_H

#include <memory>

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EventTimerShutdown, Event)

class ActorsManager : public feal::Actor
{
   public:
    ActorsManager() = default;
    ~ActorsManager() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EventTimerShutdown> pevt);

   private:
    std::vector<std::shared_ptr<feal::Actor>> actors;
    feal::Timers<ActorsManager> timers;
};

#endif  // FEAL_ACTORSMANAGER_H
