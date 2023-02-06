//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <cstdio>
#include <future>
#include <string>
#include "ActorsManager.h"
#include "ActorA.h"

#if defined (_WIN32)
#define CMDSTR "dir c:\\"
#else
#define CMDSTR "ls -al /"
#endif


feal::EventId_t EvtAccPromiseComplete::getId(void)
{
    return getIdOfType<EvtAccPromiseComplete>();
}

feal::EventId_t EvtPopenPromiseComplete::getId(void)
{
    return getIdOfType<EvtPopenPromiseComplete>();
}


void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    std::promise<std::shared_ptr<EvtAccPromiseComplete>> accumulate_promise;
    fut_acc = accumulate_promise.get_future().share();
    acc_work_thread = std::thread(&accThreadLauncher,
        this, std::move(numbers), std::move(accumulate_promise));
    subscribePromise(this, fut_acc);

    std::string str {CMDSTR};
    std::promise<std::shared_ptr<EvtPopenPromiseComplete>> popen_promise;
    fut_popen = popen_promise.get_future().share();
    popen_work_thread = std::thread(&popenThreadLauncher,
        this, std::move(str), std::move(popen_promise));
    subscribePromise(this, fut_popen);

}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EvtAccPromiseComplete> pevt)
{
    if (!pevt) return;
    printf("ActorA EvtAccPromiseComplete\n");
    printf("Results back from acc: %d\n", 
        ((EvtAccPromiseComplete*) pevt.get())->total);
    if (acc_work_thread.joinable()) acc_work_thread.join();
    count++;
    if (count >= 2)
    {
        std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtWorkDone>();
        publishEvent(pevt2);
    }
}

void ActorA::handleEvent(std::shared_ptr<EvtPopenPromiseComplete> pevt)
{
    if (!pevt) return;
    printf("ActorA EvtPopenPromiseComplete\n");
    printf("Results back from popen:\n %s\n", 
        ((EvtPopenPromiseComplete*) pevt.get())->str.c_str());
    if (popen_work_thread.joinable()) popen_work_thread.join();
    count++;
    if (count >= 2)
    {
        std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtWorkDone>();
        publishEvent(pevt2);
    }
}

void ActorA::accThreadLauncher(ActorA* ptr, std::vector<int> vec, 
    std::promise<std::shared_ptr<EvtAccPromiseComplete>> prom)
{
    if (ptr) ptr->accumulator(std::move(vec), std::move(prom));
}

void ActorA::accumulator(std::vector<int> vec,
    std::promise<std::shared_ptr<EvtAccPromiseComplete>> prom)
{
    int sum = 0;
    printf("ActorA accumulator\n");
    printf("Accumulator: Busy doing work\n");
    for(auto it = vec.begin(); it != vec.end(); ++it)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sum += *it;
        printf("Accumulator: Still busy\n");
    }
    std::shared_ptr<EvtAccPromiseComplete> sp = std::make_shared<EvtAccPromiseComplete>();
    sp.get()->total = sum;
    printf("Accumulator: Work done!\n");
    prom.set_value(sp); 
}

void ActorA::popenThreadLauncher(ActorA* ptr, std::string cmdstr, 
    std::promise<std::shared_ptr<EvtPopenPromiseComplete>> prom)
{
    if (ptr) ptr->externalcommand(std::move(cmdstr), std::move(prom));
}

void ActorA::externalcommand(std::string cmdstr,
    std::promise<std::shared_ptr<EvtPopenPromiseComplete>> prom)
{
    printf("ActorA external_command_processor\n");
    std::shared_ptr<EvtPopenPromiseComplete> sp = std::make_shared<EvtPopenPromiseComplete>();
    std::string result;
    char buf[100];
    FILE* fp;
    fp = popen(cmdstr.c_str(), "r");
    if (fp == NULL)
    {
        result = "popen error\n";
        sp.get()->str = result;
    }
    else
    {
        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            result += buf;
        }
        sp.get()->str = result;
    }
    pclose(fp);
    printf("popen: Work done!\n");
    prom.set_value(sp); 
}


