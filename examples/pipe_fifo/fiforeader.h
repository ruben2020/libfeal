//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FIFOREADER_H
#define FIFOREADER_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtRetryTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtFifoRead, EventComm)

class Fiforeader : public feal::Actor
{
   public:
    Fiforeader() = default;
    ~Fiforeader() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
    void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
    void handleEvent(std::shared_ptr<EvtFifoRead> pevt);

   private:
    void openForReading(void);
    feal::Timers<Fiforeader> timers;
    feal::FifoReader<Fiforeader> reader;
};

#endif  // FIFOREADER_H
