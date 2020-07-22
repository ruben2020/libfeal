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

feal::EventBus::~EventBus()
{
    mapEventSubscribers.clear();
    inst = nullptr;
}

void feal::EventBus::subscribeEvent(feal::EventId_t& evtid, feal::Actor* ptr)
{
    const std::lock_guard<std::mutex> lock(mtxEventBus);
    auto it = mapEventSubscribers.find(evtid);
    if (it != mapEventSubscribers.end())
    {
        bool found = false;
        for (auto itv = it->second.begin(); itv != it->second.end(); ++itv)
        {
            if ((*itv) == actorptr_t(ptr))
            {
                found = true;
                break;
            }
        }
        if (!found) it->second.push_back(actorptr_t(ptr));
    }
    else
    {
        mapEventSubscribers[evtid] = vec_actor_ptr_t { actorptr_t(ptr) };
    }
    
}

void feal::EventBus::unsubscribeEvent(feal::EventId_t& evtid, feal::Actor* ptr)
{
    const std::lock_guard<std::mutex> lock(mtxEventBus);
    auto it = mapEventSubscribers.find(evtid);
    if (it != mapEventSubscribers.end())
    {
        for (auto itv = it->second.begin(); itv != it->second.end(); )
        {
            if ((*itv) == actorptr_t(ptr))
            {
                it->second.erase(itv);
            }
            else ++itv;
        }
    }
}

void feal::EventBus::publishEvent(std::shared_ptr<Event> pevt)
{
    if (!pevt) return;
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

void feal::EventBus::resetBus(void)
{
    const std::lock_guard<std::mutex> lock(mtxEventBus);
    mapEventSubscribers.clear();
}

