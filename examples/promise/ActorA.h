/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Licensed under the Apache License, Version 2.0
 * with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You can find a copy of the License in the LICENSE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 * 
 */
 
#ifndef _FEAL_ACTOR_A_H
#define _FEAL_ACTOR_A_H

#include "feal.h"


class EvtPromiseComplete : public feal::Event
{
public:
EvtPromiseComplete() = default;
EvtPromiseComplete( const EvtPromiseComplete & ) = default;
EvtPromiseComplete& operator= ( const EvtPromiseComplete & ) = default;
~EvtPromiseComplete() = default;
feal::EventId_t getId(void);
int total = 0;
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

void handleEvent(std::shared_ptr<EvtPromiseComplete> pevt);

private:
feal::Timers<ActorA> timers;
std::thread work_thread;
std::shared_future<std::shared_ptr<EvtPromiseComplete>> fut_acc;

static void accThreadLauncher(ActorA* ptr, std::vector<int> vec, 
    std::promise<std::shared_ptr<EvtPromiseComplete>> prom);
void accumulator(std::vector<int> vec,
    std::promise<std::shared_ptr<EvtPromiseComplete>> prom);

};


#endif // _FEAL_ACTOR_A_H
