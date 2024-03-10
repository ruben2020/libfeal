//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _FIFOWRITER_H
#define _FIFOWRITER_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)


class Fifowriter : public feal::Actor
{

public:

Fifowriter() = default;
~Fifowriter() = default;

void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
void handleEvent(std::shared_ptr<EvtDelayTimer> pevt);

protected:

feal::Timers<Fifowriter> timers;

private:
int n=0;
feal::handle_t fifofd = -1;
void open_for_writing(void);
void send_something(void);

};

#endif // _FIFOWRITER_H

