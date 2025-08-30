//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_SIGNAL_H
#define FEAL_SIGNAL_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <vector>
#include <mutex>
#include <map>
#include <memory>
#include <typeindex>

namespace feal
{

class EventSignal : public Event
{
   public:
    EventSignal() = default;
    EventSignal(const EventSignal&) = default;
    EventSignal& operator=(const EventSignal&) = default;
    ~EventSignal() override = default;
    int signo = -1;
    int sicode = -1;
};

typedef std::vector<std::shared_ptr<EventSignal>> vec_evtsig_ptr_t;
typedef std::map<int, vec_evtsig_ptr_t> map_evtsig_t;

class SignalGeneric : public BaseSignal
{
   public:
    SignalGeneric() = default;
    SignalGeneric(const SignalGeneric&) = default;
    SignalGeneric& operator=(const SignalGeneric&) = default;
    ~SignalGeneric() override = default;

   protected:
    void init(void);

    template <class T>
    errenum_t registersignal(int signum)
    {
        errenum_t ee = FEAL_OK;
        std::shared_ptr<EventSignal> k = std::make_shared<T>();
        auto id = std::type_index(typeid(T));
        const std::lock_guard<std::mutex> lock(mtx_map_sig);
        auto it = map_signal.find(signum);
        if (it == map_signal.end())
        {
            vec_evtsig_ptr_t ves;
            ves.push_back(k);
            map_signal[signum] = ves;
            if (BaseSignal::doRegisterSignal(signum) == -1)
                ee = static_cast<errenum_t>(errno);
        }
        else
        {
            vec_evtsig_ptr_t ves = it->second;
            bool notinvec = true;
            for (auto itv = ves.begin(); itv != ves.end(); ++itv)
            {
                if (id == std::type_index(typeid((*itv).get())))
                {
                    notinvec = false;
                    break;
                }
            }
            if (notinvec)
            {
                ves.push_back(k);
                map_signal[signum] = ves;
                if (BaseSignal::doRegisterSignal(signum) == -1)
                    ee = static_cast<errenum_t>(errno);
            }
        }
        return ee;
    }

    template <class T>
    errenum_t deregistersignal(int signum)
    {
        errenum_t ee = FEAL_OK;
        std::shared_ptr<EventSignal> k = std::make_shared<T>();
        auto id = std::type_index(typeid(T));
        const std::lock_guard<std::mutex> lock(mtx_map_sig);
        auto it = map_signal.find(signum);
        if (it != map_signal.end())
        {
            vec_evtsig_ptr_t ves = it->second;
            for (auto itv = ves.begin(); itv != ves.end(); ++itv)
            {
                if (id == std::type_index(typeid((*itv).get())))
                {
                    ves.erase(itv);
                    break;
                }
            }
            if (ves.empty())
            {
                map_signal.erase(it);
                if (BaseSignal::doDeregisterSignal(signum) == -1)
                    ee = static_cast<errenum_t>(errno);
            }
            else
                map_signal[signum] = ves;
        }
        return ee;
    }

    static void sighandler(int signum, int sicode);

   private:
    static map_evtsig_t map_signal;
    static std::mutex mtx_map_sig;
};

template <class Y>
class Signal : public SignalGeneric
{
   public:
    Signal() = default;
    Signal(const Signal&) = default;
    Signal& operator=(const Signal&) = default;
    ~Signal() override = default;

    void init(Y* p)
    {
        actorptr = p;
        p->addTool(this);
        SignalGeneric::init();
    }

    template <typename T>
    errenum_t registerSignal(int signum)
    {
        T k;
        EventBus::getInstance().registerEventCloner<T>();
        actorptr->subscribeEvent(actorptr, k);
        return SignalGeneric::registersignal<T>(signum);
    }

    template <typename T>
    errenum_t deregisterSignal(int signum)
    {
        T k;
        actorptr->unsubscribeEvent(k);
        return SignalGeneric::deregistersignal<T>(signum);
    }

   private:
    Y* actorptr = nullptr;
};

}  // namespace feal

#endif  // FEAL_SIGNAL_H
