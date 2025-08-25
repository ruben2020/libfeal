//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef _ACTORA_H
#define _ACTORA_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtFDMReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtFDMErr, EventComm)

class ActorA : public feal::Actor
{
   public:
    ActorA() = default;
    ~ActorA() = default;

    void initActor(void);
    void startActor(void);
    void pauseActor(void);
    void shutdownActor(void);

    void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
    void handleEvent(std::shared_ptr<EvtDelayTimer> pevt);
    void handleEvent(std::shared_ptr<EvtFDMReadAvail> pevt);
    void handleEvent(std::shared_ptr<EvtFDMErr> pevt);

   protected:
    feal::Timers<ActorA> timers;
    feal::FileDirMon<ActorA> fdmon;

   private:
    int n = 0;
    int wnum1 = -1;
    int wnum2 = -1;
};

#endif  // _ACTORA_H
