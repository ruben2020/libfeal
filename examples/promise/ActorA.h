//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_ACTOR_A_H
#define FEAL_ACTOR_A_H

#include "feal.h"

class EvtAccPromiseComplete : public feal::Event
{
   public:
    EvtAccPromiseComplete() = default;
    EvtAccPromiseComplete(const EvtAccPromiseComplete&) = default;
    EvtAccPromiseComplete& operator=(const EvtAccPromiseComplete&) = default;
    ~EvtAccPromiseComplete() override = default;
    int total = 0;
};

class EvtPopenPromiseComplete : public feal::Event
{
   public:
    EvtPopenPromiseComplete() = default;
    EvtPopenPromiseComplete(const EvtPopenPromiseComplete&) = default;
    EvtPopenPromiseComplete& operator=(const EvtPopenPromiseComplete&) = default;
    ~EvtPopenPromiseComplete() override = default;
    std::string str;
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

    void handleEvent(std::shared_ptr<EvtAccPromiseComplete> pevt);
    void handleEvent(std::shared_ptr<EvtPopenPromiseComplete> pevt);

   private:
    feal::Timers<ActorA> timers;
    std::atomic_uint count{0};
    std::thread acc_work_thread;
    std::thread popen_work_thread;
    std::shared_future<std::shared_ptr<EvtAccPromiseComplete>> fut_acc;
    std::shared_future<std::shared_ptr<EvtPopenPromiseComplete>> fut_popen;

    static void accThreadLauncher(ActorA* ptr, std::vector<int> vec,
                                  std::promise<std::shared_ptr<EvtAccPromiseComplete>> prom);
    void accumulator(std::vector<int> vec,
                     std::promise<std::shared_ptr<EvtAccPromiseComplete>> prom);

    static void popenThreadLauncher(ActorA* ptr, std::string cmdstr,
                                    std::promise<std::shared_ptr<EvtPopenPromiseComplete>> prom);
    void externalcommand(std::string cmdstr,
                         std::promise<std::shared_ptr<EvtPopenPromiseComplete>> prom);
};

#endif  // FEAL_ACTOR_A_H
