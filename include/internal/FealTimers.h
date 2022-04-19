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
 
#ifndef _FEAL_TIMERS_H
#define _FEAL_TIMERS_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <memory>
#include <chrono>

namespace feal
{

template<typename Y>
class Timers : public Tool
{
public:

Timers() = default;
virtual ~Timers() = default;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
}

void shutdownTool(void)
{
    finalizeAllTimers();
}

template< typename T, class D1, class D2 >
void startTimer(const D1& oneshot_time, const D2& repeat_time)
{
    EventId_t id = Event::getIdOfType<T>();
    auto it = mapTimers.find(id);
    if (it != mapTimers.end())
    {
        it->second.get()->startTimer(oneshot_time, repeat_time);
    }
    else
    {
        createTimer<T>();
        it = mapTimers.find(id);
        if (it != mapTimers.end())
            it->second.get()->startTimer(oneshot_time, repeat_time);
    }
}

template< typename T, class D1 >
void startTimer(const D1& oneshot_time)
{
    EventId_t id = Event::getIdOfType<T>();
    auto it = mapTimers.find(id);
    if (it != mapTimers.end())
    {
        it->second.get()->startTimer(oneshot_time, std::chrono::seconds(0));
    }
    else
    {
        createTimer<T>();
        it = mapTimers.find(id);
        if (it != mapTimers.end())
            it->second.get()->startTimer(oneshot_time, std::chrono::seconds(0));
    }
}

template< typename T >
void stopTimer(void)
{
    EventId_t id = Event::getIdOfType<T>();
    auto it = mapTimers.find(id);
    if (it != mapTimers.end())
    {
        it->second.get()->stopTimer();
    }
}

template< typename T, class D >
bool setTimerRepeat(const D& repeat_time)
{
    bool ret = false;
    EventId_t id = Event::getIdOfType<T>();
    auto it = mapTimers.find(id);
    if (it != mapTimers.end())
    {
        ret = it->second.get()->setTimerRepeat(repeat_time);
    }
    else
    {
        createTimer<T>();
        it = mapTimers.find(id);
        if (it != mapTimers.end())
            ret = it->second.get()->setTimerRepeat(repeat_time);
    }
    return ret;
}

template< typename T >
std::chrono::seconds getTimerRepeat(void)
{
    EventId_t id = Event::getIdOfType<T>();
    auto sec = std::chrono::seconds(0);
    auto it = mapTimers.find(id);
    if (it != mapTimers.end())
    {
       sec = it->second.get()->getTimerRepeat();
    }
    else
    {
        createTimer<T>();
        it = mapTimers.find(id);
        if (it != mapTimers.end())
           sec = it->second.get()->getTimerRepeat();
    }
    return sec;
}

void stopAllTimers(void)
{
    for (auto it = mapTimers.begin(); it != mapTimers.end(); ++it)
    {
        it->second.get()->stopTimer();
    }
}

void finalizeAllTimers(void)
{
    for (auto it = mapTimers.begin(); it != mapTimers.end(); ++it)
    {
        it->second.get()->finalizeTimer();
    }
    mapTimers.clear();
}

private:

Y* actorptr = nullptr;
std::map<EventId_t, std::unique_ptr<Timer>> mapTimers;

template<typename T>
void createTimer(void)
{
    if (actorptr == nullptr) return;
    EventId_t id = Event::getIdOfType<T>();
    T inst;
    actorptr->addEvent(actorptr, inst);
    auto it = mapTimers.find(id);
    if (it == mapTimers.end())
    {
        std::shared_ptr<Event> spevt = std::make_shared<T>();
        EventBus::getInstance().registerEventCloner<T>();
        std::weak_ptr<Actor> wkact = actorptr->shared_from_this();
        spevt.get()->setSender(wkact);
        Timer *timi = new Timer();
        timi->setTimerEvent(spevt);
        timi->initTimer();
        mapTimers[id] = std::unique_ptr<Timer>(timi);
    }
}

};


} // namespace feal


#endif // _FEAL_TIMERS_H
