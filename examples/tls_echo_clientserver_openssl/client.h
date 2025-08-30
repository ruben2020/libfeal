//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef CLIENT_H
#define CLIENT_H

#include <openssl/bio.h>
#include <openssl/ssl.h>

#include "feal.h"

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtDelayTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtRetryTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtSigInt, EventSignal)
FEAL_EVENT_DEFAULT_DECLARE(EvtConnectedToServer, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDataReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDataWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtConnectionShutdown, EventComm)

class Client : public feal::Actor
{
   public:
    Client() = default;
    ~Client() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
    void handleEvent(std::shared_ptr<EvtDelayTimer> pevt);
    void handleEvent(std::shared_ptr<EvtRetryTimer> pevt);
    void handleEvent(std::shared_ptr<EvtConnectedToServer> pevt);
    void handleEvent(std::shared_ptr<EvtDataReadAvail> pevt);
    void handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt);
    void handleEvent(std::shared_ptr<EvtConnectionShutdown> pevt);
    void handleEvent(std::shared_ptr<EvtSigInt> pevt);

   protected:
    feal::Timers<Client> timers;
    feal::Stream<Client> stream;
    feal::Signal<Client> signal;
    virtual void connectToServer(void);

   private:
    int n = 0;
    char buf[20000];
    SSL_CTX *ctx = nullptr;
    SSL *ssl = nullptr;
    BIO *bio = nullptr;
    bool sslconnect_pending = false;
    bool sslread_want_write = false;
    bool sslwrite_want_read = false;
    bool sslwrite_want_write = false;
    bool sslshutdown_pending = false;
    bool sslshutdown_complete = false;

    int setupSslctx(void);
    void sendSomething(void);
    int performSslconnect(void);
    int performRead(void);
    int performWrite(int num);
    int performSslshutdown(void);
};

#endif  // _CLIENT_H
