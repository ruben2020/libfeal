//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef _FEAL_ACTOR_B_H
#define _FEAL_ACTOR_B_H

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
    ~EventCabbage() = default;

    int get_cabbages(void);

   private:
    void set_cabbages(const int& num);
    int cabbages = 0;
};

FEAL_EVENT_DEFAULT_DECLARE(EventTimerB1, Event)
FEAL_EVENT_DEFAULT_DECLARE(EventTimerB2, Event)

class ActorB : public feal::Actor
{
   public:
    ActorB() = default;
    ~ActorB() = default;

    void initActor(void);
    void startActor(void);
    void pauseActor(void);
    void shutdownActor(void);

    void handleEvent(std::shared_ptr<EvtNotifyVege> pevt);
    void handleEvent(std::shared_ptr<EvtNotifyFruit> pevt);
    void handleEvent(std::shared_ptr<EventTimerB1> pevt);
    void handleEvent(std::shared_ptr<EventTimerB2> pevt);

   private:
    feal::Timers<ActorB> timers;
};

#endif  // _FEAL_ACTOR_B_H
