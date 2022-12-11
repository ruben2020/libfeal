//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR LGPL-2.1-or-later
//
 
#ifndef _FEAL_SIGNAL_H
#define _FEAL_SIGNAL_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <vector>
#include <mutex>
#include <map>
#include <memory>

namespace feal
{

class EventSignal : public Event
{
public:
EventSignal() = default;
EventSignal( const EventSignal & ) = default;
EventSignal& operator= ( const EventSignal & ) = default;
~EventSignal() = default;
int signo = -1;
int sicode = -1;
};

typedef std::vector<std::shared_ptr<EventSignal>> vec_evtsig_ptr_t;
typedef std::map<int, vec_evtsig_ptr_t> map_evtsig_t;

class SignalGeneric : public BaseSignal
{
public:
SignalGeneric() = default;
SignalGeneric( const SignalGeneric & ) = default;
SignalGeneric& operator= ( const SignalGeneric & ) = default;
~SignalGeneric() = default;

protected:

void init(void);

template<class T>
errenum registersignal(int signum)
{
    errenum ee = FEAL_OK;
    std::shared_ptr<EventSignal> k = std::make_shared<T>();
    EventId_t id = Event::getIdOfType<T>();
    const std::lock_guard<std::mutex> lock(mtxMapSig);
    auto it = mapSignal.find(signum);
    if (it == mapSignal.end())
    {
        vec_evtsig_ptr_t ves;
        ves.push_back(k);
        mapSignal[signum] = ves;
        if (BaseSignal::do_registersignal(signum) == -1) 
            ee = static_cast<errenum>(errno);
    }
    else
    {
        vec_evtsig_ptr_t ves = it->second;
        bool notinvec = true;
        for (auto itv = ves.begin(); itv != ves.end(); ++itv)
        {
            if (id == (*itv).get()->getId())
            {
                notinvec = false;
                break;
            }
        }
        if (notinvec)
        {
            ves.push_back(k);
            mapSignal[signum] = ves;
            if (BaseSignal::do_registersignal(signum) == -1) 
                ee = static_cast<errenum>(errno);
        }
    }
    return ee;
}

template<class T>
errenum deregistersignal(int signum)
{
    errenum ee = FEAL_OK;
    std::shared_ptr<EventSignal> k = std::make_shared<T>();
    EventId_t id = Event::getIdOfType<T>();
    const std::lock_guard<std::mutex> lock(mtxMapSig);
    auto it = mapSignal.find(signum);
    if (it != mapSignal.end())
    {
        vec_evtsig_ptr_t ves = it->second;
        for (auto itv = ves.begin(); itv != ves.end(); ++itv)
        {
            if (id == (*itv).get()->getId())
            {
                ves.erase(itv);
                break;
            }
        }
        if (ves.empty())
        {
            mapSignal.erase(it);
            if (BaseSignal::do_deregistersignal(signum) == -1)
                ee = static_cast<errenum>(errno);
        }
        else mapSignal[signum] = ves;
    }
    return ee;
}

static void sighandler(int signum, int sicode);

private:

static map_evtsig_t mapSignal;
static std::mutex mtxMapSig;

};



template<class Y>
class Signal : public SignalGeneric
{
public:
Signal() = default;
Signal( const Signal & ) = default;
Signal& operator= ( const Signal & ) = default;
~Signal() = default;

void init(Y* p)
{
    actorptr = p;
    p->addTool(this);
    SignalGeneric::init();
}

template<typename T>
errenum registerSignal(int signum)
{
    T k;
    EventBus::getInstance().registerEventCloner<T>();
    actorptr->subscribeEvent(actorptr, k);
    return SignalGeneric::registersignal<T>(signum);
}

template<typename T>
errenum deregisterSignal(int signum)
{
    T k;
    actorptr->unsubscribeEvent(k);
    return SignalGeneric::deregistersignal<T>(signum);
}

private:

Y* actorptr = nullptr;

};


} // namespace feal


#endif // _FEAL_SIGNAL_H
