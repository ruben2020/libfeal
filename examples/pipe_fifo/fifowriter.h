//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FIFOWRITER_H
#define FIFOWRITER_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)

class Fifowriter : public feal::Actor
{
   public:
    Fifowriter() = default;
    ~Fifowriter() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
    void handleEvent(std::shared_ptr<EvtDelayTimer> pevt);

   protected:
    feal::Timers<Fifowriter> timers;

   private:
    int n = 0;
    feal::handle_t fifofd = -1;
    void openForWriting(void);
    void sendSomething(void);
};

#endif  // FIFOWRITER_H
