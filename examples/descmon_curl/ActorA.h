//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef ACTORA_H
#define ACTORA_H

#include <curl/curl.h>

#include "feal.h"

class ActorA;

struct CurlContext
{
    curl_socket_t sockfd;
    ActorA *act;
};

FEAL_EVENT_DEFAULT_DECLARE(EvtEndTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlTimer, Event)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlMReadAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlMWriteAvail, EventComm)
FEAL_EVENT_DEFAULT_DECLARE(EvtCurlMErr, EventComm)

class ActorA : public feal::Actor
{
   public:
    ActorA() = default;
    ~ActorA() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EvtCurlTimer> pevt);
    void handleEvent(std::shared_ptr<EvtEndTimer> pevt);
    void handleEvent(std::shared_ptr<EvtCurlMReadAvail> pevt);
    void handleEvent(std::shared_ptr<EvtCurlMWriteAvail> pevt);
    void handleEvent(std::shared_ptr<EvtCurlMErr> pevt);

    static int cbSocket(CURL *easy, curl_socket_t s, int action, ActorA *act, void *socketp);
    static int cbTimeout(CURLM *multi, long timeout_ms, ActorA *act);

   protected:
    feal::Timers<ActorA> timers;
    feal::DescMon<ActorA> dmon;

   private:
    CURLM *multi = nullptr;
    std::atomic_int remaining = 0;
    void addDownload(const char *url, int num);
    void checkMultiInfo(void);
    struct CurlContext *createCurlContext(curl_socket_t sockfd, ActorA *act);
};

#endif  // _ACTORA_H
