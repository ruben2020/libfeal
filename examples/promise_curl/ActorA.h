//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_ACTOR_A_H
#define FEAL_ACTOR_A_H

#include <curl/curl.h>

#include "feal.h"

class EvtCurlPromiseComplete : public feal::Event
{
   public:
    EvtCurlPromiseComplete() = default;
    EvtCurlPromiseComplete(const EvtCurlPromiseComplete&) = default;
    EvtCurlPromiseComplete& operator=(const EvtCurlPromiseComplete&) = default;
    ~EvtCurlPromiseComplete() override = default;
    curl_off_t bytes;
    bool success = false;
};

class ActorA : public feal::Actor
{
   public:
    ActorA() = default;
    ~ActorA() override = default;

    void initActor(void) override;
    void startActor(void) override;
    void pauseActor(void) override;
    void shutdownActor(void) override;

    void handleEvent(std::shared_ptr<EvtCurlPromiseComplete> pevt);

   private:
    feal::Timers<ActorA> timers;

    std::thread curl_work_thread;
    std::shared_future<std::shared_ptr<EvtCurlPromiseComplete>> fut_curl;

    static void curlThreadLauncher(ActorA* ptr, std::string url, std::string filepath,
                                   std::promise<std::shared_ptr<EvtCurlPromiseComplete>> prom);
    void curlLoop(std::string url, std::string filepath,
                  std::promise<std::shared_ptr<EvtCurlPromiseComplete>> prom);
};

#endif  // FEAL_ACTOR_A_H
