//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#ifndef _FIFOREADER_H
#define _FIFOREADER_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtRetryTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtFifoRead, EventComm)


class Fiforeader : public feal::Actor
{

public:

Fiforeader() = default;
~Fiforeader() = default;

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
void handleEvent(std::shared_ptr<EvtFifoRead> pevt);

private:
void open_for_reading(void);
feal::Timers<Fiforeader> timers;
feal::FifoReader<Fiforeader> reader;

};

#endif // _FIFOREADER_H
