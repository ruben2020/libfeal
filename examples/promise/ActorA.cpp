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
 
#include <cstdio>
#include <future>
#include "ActorsManager.h"
#include "ActorA.h"

feal::EventId_t EvtPromiseComplete::getId(void)
{
    return getIdOfType<EvtPromiseComplete>();
}

void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");
    std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    std::promise<std::shared_ptr<EvtPromiseComplete>> accumulate_promise;
    fut_acc = accumulate_promise.get_future().share();
    work_thread = std::thread(&accThreadLauncher,
        this, std::move(numbers), std::move(accumulate_promise));
    subscribePromise(this, fut_acc);
}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EvtPromiseComplete> pevt)
{
    if (!pevt) return;
    printf("ActorA EvtPromiseComplete\n");
    printf("Results back from acc: %d\n", 
        ((EvtPromiseComplete*) pevt.get())->total);
    if (work_thread.joinable()) work_thread.join();
    std::shared_ptr<feal::Event> pevt2 = std::make_shared<EvtWorkDone>();
    publishEvent(pevt2);
}

void ActorA::accThreadLauncher(ActorA* ptr, std::vector<int> vec, 
    std::promise<std::shared_ptr<EvtPromiseComplete>> prom)
{
    if (ptr) ptr->accumulator(std::move(vec), std::move(prom));
}

void ActorA::accumulator(std::vector<int> vec,
    std::promise<std::shared_ptr<EvtPromiseComplete>> prom)
{
    int sum = 0;
    printf("ActorA accumulator\n");
    printf("Busy doing work\n");
    for(auto it = vec.begin(); it != vec.end(); ++it)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sum += *it;
        printf("Still busy\n");
    }
    std::shared_ptr<EvtPromiseComplete> sp = std::make_shared<EvtPromiseComplete>();
    sp.get()->total = sum;
    printf("Work done!\n");
    prom.set_value(sp); 
}

