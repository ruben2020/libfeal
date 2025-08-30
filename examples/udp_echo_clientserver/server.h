//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef SERVER_H
#define SERVER_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtRetryTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtSockErr, EventComm)

class Server : public feal::Actor
{
   public:
    Server() = default;
    ~Server() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
    void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
    void handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt);
    void handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt);
    void handleEvent(std::shared_ptr<EvtSockErr> pevt);

   protected:
    feal::Timers<Server> timers;
    feal::Datagram<Server> dgram;
    virtual void startListening(void);

   private:
};

#endif  // _SERVER_H
