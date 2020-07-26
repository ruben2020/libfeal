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
friend class Actor;

public:
Timer() = default;
Timer( const Timer & ) = default;
Timer& operator= ( const Timer & ) = default;
~Timer();

protected:
std::thread timerThread;

private:
void stopTimer(void);
void finalizeTimer(void);
void initTimer(void);

template< class D >
void startTimer(const D& rel_time)
{
    tpoint = std::chrono::steady_clock::now() + rel_time;
    stopTimer();
    timerActive = true;
    timerPeriodic = false;
    cvTimer.notify_all();
}

template< class D >
void startTimerPeriodic(const D& rel_time)
{
    tpoint = std::chrono::steady_clock::now() + rel_time;
    stopTimer();
    timerActive = true;
    timerPeriodic = true;
    secs = std::chrono::duration_cast<std::chrono::seconds>(rel_time);
    cvTimer.notify_all();
}

std::atomic_bool timerValid {true};
std::atomic_bool timerActive {false};
std::atomic_bool timerDormant {false};
std::atomic_bool timerPeriodic {false};
std::mutex mtxTimer;
std::condition_variable cvTimer;
std::chrono::time_point<std::chrono::steady_clock> tpoint;
std::chrono::seconds secs;
std::shared_ptr<Event> timerEvent;

static void timerLoopLauncher(Timer *p);
void timerLoop(void);
void setTimerEvent(std::shared_ptr<Event> timEvt);

};


} // namespace feal



#endif // _FEAL_TIMER_H
