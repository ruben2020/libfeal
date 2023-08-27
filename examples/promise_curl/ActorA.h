//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"
#include <curl/curl.h>

class EvtCurlPromiseComplete : public feal::Event
{
public:
EvtCurlPromiseComplete() = default;
EvtCurlPromiseComplete( const EvtCurlPromiseComplete & ) = default;
EvtCurlPromiseComplete& operator= ( const EvtCurlPromiseComplete & ) = default;
~EvtCurlPromiseComplete() = default;
feal::EventId_t getId(void);
curl_off_t bytes;
bool success = false;
};


class ActorA : public feal::Actor
{

public:

ActorA() = default;
~ActorA() = default;


void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtCurlPromiseComplete> pevt);

private:
feal::Timers<ActorA> timers;

std::thread curl_work_thread;
std::shared_future<std::shared_ptr<EvtCurlPromiseComplete>> fut_curl;

static void curlThreadLauncher(ActorA* ptr, 
    std::string url, std::string filepath, 
    std::promise<std::shared_ptr<EvtCurlPromiseComplete>> prom);
void curl_loop(std::string url, std::string filepath,
    std::promise<std::shared_ptr<EvtCurlPromiseComplete>> prom);

};


#endif // _FEAL_ACTOR_A_H
