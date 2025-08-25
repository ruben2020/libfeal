//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef _CLIENT_H
#define _CLIENT_H

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDgramWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtSockErr, EventComm)

class Client : public feal::Actor
{
   public:
    Client() = default;
    ~Client() = default;

    void initActor(void);
    void startActor(void);
    void pauseActor(void);
    void shutdownActor(void);

    void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
    void handleEvent(std::shared_ptr<EvtDelayTimer> pevt);
    void handleEvent(std::shared_ptr<EvtDgramReadAvail> pevt);
    void handleEvent(std::shared_ptr<EvtDgramWriteAvail> pevt);
    void handleEvent(std::shared_ptr<EvtSockErr> pevt);

   protected:
    feal::Timers<Client> timers;
    feal::Datagram<Client> dgram;
    feal::sockaddr_all serveraddr;
    feal::socklen_t serverport;
    virtual void send_to_server(void);

   private:
    int n = 0;

    void send_something(void);
};

#endif  // _CLIENT_H
