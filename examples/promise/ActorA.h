/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Dual-licensed under the Apache License, Version 2.0, and
 * the GNU General Public License, Version 2.0;
 * you may not use this file except in compliance
 * with either one of these licenses.
 *
 * You can find copies of these licenses in the included
 * LICENSE-APACHE2 and LICENSE-GPL2 files.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under these licenses is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the license texts for the specific language governing permissions
 * and limitations under the licenses.
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
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