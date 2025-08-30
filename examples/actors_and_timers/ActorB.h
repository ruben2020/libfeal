//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_ACTOR_B_H
#define FEAL_ACTOR_B_H

#include "feal.h"

class EvtNotifyVege;
class EvtNotifyFruit;

class EventCabbage : public feal::Event
{
    friend class ActorB;

   public:
    EventCabbage() = default;
    EventCabbage(const EventCabbage&) = default;
    EventCabbage& operator=(const EventCabbage&) = default;
    ~EventCabbage() override = default;

    int getCabbages(void);

   private:
    void setCabbages(const int& num);
    int cabbages = 0;
};

FEAL_EVENT_DEFAULT_DECLARE(EventTimerB1, Event)
FEAL_EVENT_DEFAULT_DECLARE(EventTimerB2, Event)

class ActorB : public feal::Actor
{
   public:
    ActorB() = default;
    ~ActorB() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EvtNotifyVege> pevt);
    void handleEvent(std::shared_ptr<EvtNotifyFruit> pevt);
    void handleEvent(std::shared_ptr<EventTimerB1> pevt);
    void handleEvent(std::shared_ptr<EventTimerB2> pevt);

   private:
    feal::Timers<ActorB> timers;
};

#endif  // FEAL_ACTOR_B_H
