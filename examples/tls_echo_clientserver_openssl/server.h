//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef SERVER_H
#define SERVER_H

#include <openssl/bio.h>
#include <openssl/ssl.h>

#include "feal.h"

class ClientHandler;
class EvtClientDisconnected;

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtRetryTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtSigInt, EventSignal)
FEAL_EVENT_DEFAULT_DECLARE(EvtIncomingConn, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtServerShutdown, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtConnectionShutdown, EventComm)

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
    void handleEvent(std::shared_ptr<EvtIncomingConn> pevt);
    void handleEvent(std::shared_ptr<EvtServerShutdown> pevt);
    void handleEvent(std::shared_ptr<EvtClientDisconnected> pevt);
    void handleEvent(std::shared_ptr<EvtSigInt> pevt);

   protected:
    feal::Timers<Server> timers;
    feal::Stream<Server> stream;
    feal::Signal<Server> signal;
    virtual void startServer(void);
    virtual void printClientAddress(feal::handle_t fd);
    virtual void getClientAddress(feal::handle_t fd, char* addr, int addrbuflen);

   private:
    std::map<feal::handle_t, std::shared_ptr<ClientHandler>> mapch;
    SSL_CTX* ctx = nullptr;

    int setupSslctx(void);
};

#endif  // _SERVER_H
