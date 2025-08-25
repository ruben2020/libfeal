//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef _CLIENTHANDLER_H
#define _CLIENTHANDLER_H

#include <openssl/bio.h>
#include <openssl/ssl.h>

#include "feal.h"

class Server;

class EvtClientDisconnected : public feal::Event
{
    friend class ClientHandler;

   public:
    EvtClientDisconnected() = default;
    EvtClientDisconnected(const EvtClientDisconnected &) = default;
    EvtClientDisconnected &operator=(const EvtClientDisconnected &) = default;
    ~EvtClientDisconnected() = default;

    feal::handle_t fd = -1;

   private:
};

FEAL_EVENT_DEFAULT_DECLARE(EvtDataReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtDataWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtClientShutdown, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtClientSSLShutdown, EventComm)

class ClientHandler : public feal::Actor
{
   public:
    ClientHandler() = default;
    ~ClientHandler() = default;
    void setParam(feal::Stream<Server> *p, feal::handle_t fd, char *s, SSL_CTX *ctx1);

    void initActor(void);
    void startActor(void);
    void pauseActor(void);
    void shutdownActor(void);

    void handleEvent(std::shared_ptr<EvtDataReadAvail> pevt);
    void handleEvent(std::shared_ptr<EvtDataWriteAvail> pevt);
    void handleEvent(std::shared_ptr<EvtClientShutdown> pevt);
    void handleEvent(std::shared_ptr<EvtClientSSLShutdown> pevt);

   private:
    char buf[20000];
    feal::Stream<Server> *stream = nullptr;
    feal::handle_t sockfd = -1;
    std::string addrstr;
    SSL_CTX *ctx = nullptr;
    SSL *ssl = nullptr;
    BIO *client_bio = nullptr;
    bool sslaccept_pending = false;
    bool sslread_want_write = false;
    bool sslwrite_want_read = false;
    bool sslwrite_want_write = false;
    bool sslshutdown_pending = false;
    bool sslshutdown_complete = false;

    int perform_sslaccept(void);
    int perform_read(void);
    int perform_write(int num);
    int perform_sslshutdown(void);
};

#endif  // _CLIENTHANDLER_H
