//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include "feal.h"


feal::EventBus* feal::EventBus::inst = nullptr;

feal::EventBus& feal::EventBus::getInstance(void)
{
    if (inst == nullptr)
    {
        inst = new EventBus();
    }
    return *inst;
}

void feal::EventBus::destroyInstance(void)
{
    if (inst) inst->resetBus();
    delete inst;
    inst = nullptr;
}

feal::EventBus::~EventBus()
{
}

void feal::EventBus::subscribeEvent(const feal::EventId_t& evtid, feal::actorptr_t ptr)
{
    const std::lock_guard<std::mutex> lock(mtxEventBus);
    auto it = mapEventSubscribers.find(evtid);
    if (it != mapEventSubscribers.end())
    {
        bool found = false;
        for (auto itv = it->second.begin(); itv != it->second.end(); ++itv)
        {
            if ((*itv) == ptr)
            {
                found = true;
                break;
            }
        }
        if (!found) it->second.push_back(ptr);
    }
    else
    {
        mapEventSubscribers[evtid] = vec_actor_ptr_t { ptr };
    }
    
}

void feal::EventBus::unsubscribeEvent(const feal::EventId_t& evtid, feal::actorptr_t ptr)
{
    const std::lock_guard<std::mutex> lock(mtxEventBus);
    auto it = mapEventSubscribers.find(evtid);
    if (it != mapEventSubscribers.end())
    {
        for (auto itv = it->second.begin(); itv != it->second.end(); )
        {
            if ((*itv) == ptr)
            {
                it->second.erase(itv);
            }
            else ++itv;
        }
    }
}

void feal::EventBus::publishEvent(std::shared_ptr<Event> pevt)
{
    if ((!pevt)|| (eventBusOff)) return;
    const std::lock_guard<std::mutex> lock(mtxEventBus);
    auto it = mapEventSubscribers.find(pevt.get()->getId());
    if (it != mapEventSubscribers.end())
    {
        for (auto itv = it->second.begin(); itv !=  it->second.end(); ++itv)
        {
            (**itv).receiveEvent(pevt);
        }
        
    }
}

std::shared_ptr<feal::Event> feal::EventBus::cloneEvent(std::shared_ptr<feal::Event> p)
{
    const std::lock_guard<std::mutex> lock(mtxEventBus);
    auto it = mapEventCloners.find(p.get()->getId());
    if (it != mapEventCloners.end())
    {
        return it->second();
    }
    else
    {
        printf("Unable to clone event\n");
        return p;
    }
}

void feal::EventBus::stopBus(void)
{
    eventBusOff = true;
}

void feal::EventBus::resetBus(void)
{
    for (auto it = mapEventSubscribers.begin(); it != mapEventSubscribers.end(); ++it)
    {
        it->second.clear();
    }
    mapEventSubscribers.clear();
    mapEventCloners.clear();
}

