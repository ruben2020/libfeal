//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef _FEAL_TIMER_H
#define _FEAL_TIMER_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <memory>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <functional>
#include <chrono>
#include <string>

namespace feal
{

class Event;
class Actor;

class Timer
{
   public:
    Timer() = default;
    Timer(const Timer&) = default;
    Timer& operator=(const Timer&) = default;
    ~Timer();

    std::thread timerThread;

    void stopTimer(void);
    void finalizeTimer(void);
    void initTimer(void);

    template <class D1, class D2>
    void startTimer(const D1& oneshot_time, const D2& repeat_time)
    {
        stopTimer();
        mtxTimerVar.lock();
        tpoint = std::chrono::steady_clock::now() + oneshot_time;
        secs = std::chrono::duration_cast<std::chrono::seconds>(repeat_time);
        mtxTimerVar.unlock();
        timerActive = true;
        timerRepeat = (repeat_time > std::chrono::seconds(0));
        cvTimer.notify_all();
    }

    template <class D>
    bool setTimerRepeat(const D& repeat_time)
    {
        bool ret = timerRepeat;
        if (ret)
        {
            mtxTimerVar.lock();
            secs = std::chrono::duration_cast<std::chrono::seconds>(repeat_time);
            mtxTimerVar.unlock();
        }
        return ret;
    }

    std::chrono::seconds getTimerRepeat(void);
    void setTimerEvent(std::shared_ptr<Event> timEvt);

   private:
    std::atomic_bool timerValid{true};
    std::atomic_bool timerActive{false};
    std::atomic_bool timerDormant{false};
    std::atomic_bool timerRepeat{false};
    std::mutex mtxTimer;
    std::mutex mtxTimerVar;
    std::condition_variable cvTimer;
    std::chrono::time_point<std::chrono::steady_clock> tpoint;
    std::chrono::seconds secs;
    std::shared_ptr<Event> timerEvent;

    static void timerLoopLauncher(Timer* p);
    void timerLoop(void);
};

}  // namespace feal

#endif  // _FEAL_TIMER_H
