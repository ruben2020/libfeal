//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_ACTOR_A_H
#define FEAL_ACTOR_A_H

#include "feal.h"

class EventCabbage;

class EvtNotifyVege : public feal::Event
{
    friend class ActorA;

   public:
    EvtNotifyVege() = default;
    EvtNotifyVege(const EvtNotifyVege&) = default;
    EvtNotifyVege& operator=(const EvtNotifyVege&) = default;
    ~EvtNotifyVege() override = default;

    int getCarrots(void);
    int getTomatoes(void);

   private:
    void setCarrots(const int& num);
    void setTomatoes(const int& num);
    int carrots = 0;
    int tomatoes = 0;
};

class EvtNotifyFruit : public feal::Event
{
    friend class ActorA;

   public:
    EvtNotifyFruit() = default;
    EvtNotifyFruit(const EvtNotifyFruit&) = default;
    EvtNotifyFruit& operator=(const EvtNotifyFruit&) = default;
    ~EvtNotifyFruit() override = default;

    int getApples(void);
    int getOranges(void);

   private:
    void setApples(const int& num);
    void setOranges(const int& num);
    int apples = 0;
    int oranges = 0;
};

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

    void handleEvent(std::shared_ptr<EventCabbage> pevt);
    void handleEvent(std::shared_ptr<EventTimerA1> pevt);

   private:
    feal::Timers<ActorA> timers;
};

#endif  // FEAL_ACTOR_A_H
