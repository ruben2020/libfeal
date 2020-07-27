#ifndef _FEAL_ACTOR_H
#define _FEAL_ACTOR_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <functional>
#include <map>
#include <vector>

namespace feal
{

class Event;

class EventStartActor : public Event
{
public:
EventStartActor() = default;
EventStartActor( const EventStartActor & ) = default;
EventStartActor& operator= ( const EventStartActor & ) = default;
~EventStartActor() = default;
EventId_t getId(void);
};

class EventPauseActor : public Event
{
public:
EventPauseActor() = default;
EventPauseActor( const EventPauseActor & ) = default;
EventPauseActor& operator= ( const EventPauseActor & ) = default;
~EventPauseActor() = default;
EventId_t getId(void);
};


class Actor : public std::enable_shared_from_this<Actor>
{
public:

Actor() = default;
virtual ~Actor() = default;

void init(void);
void start(void);
void pause(void);
void shutdown(void);
void receiveEvent(std::shared_ptr<Event> pevt);
bool isActive(void);
void wait_for_shutdown(void);

protected:

virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);
void publishEvent(std::shared_ptr<Event> pevt);

template<typename T, typename Y>
void subscribeEvent(Y* p)
{
    EventId_t id = Event::getIdOfType<T>();
    auto it = mapEventHandlers.find(id);
    if (it == mapEventHandlers.end())
    {
        mapEventHandlers.insert(std::make_pair(id,
                [p](std::shared_ptr<Event> fe)
                    {  p->handleEvent(std::dynamic_pointer_cast<T>(fe));  }
                )
            );
    }
    EventBus::getInstance().subscribeEvent(id, shared_from_this());
}

template<typename T, typename Y>
void registerTimer(Y* p)
{
    EventId_t id = Event::getIdOfType<T>();
    auto ite = mapEventHandlers.find(id);
    if (ite == mapEventHandlers.end())
    {
        mapEventHandlers.insert(std::make_pair(id,
                [p](std::shared_ptr<Event> fe)
                    {  p->handleEvent(std::dynamic_pointer_cast<T>(fe));  }
                )
            );
    }
    auto it = mapTimers.find(id);
    if (it == mapTimers.end())
    {
        std::shared_ptr<Event> spevt = std::make_shared<T>();
        std::weak_ptr<Actor> wkact = shared_from_this();
        spevt.get()->setSender(wkact);
        Timer *timi = new Timer();
        timi->setTimerEvent(spevt);
        timi->initTimer();
        mapTimers[id] = std::unique_ptr<Timer>(timi);
    }
}

template<typename T>
void unsubscribeEvent(void)
{
    EventId_t id = Event::getIdOfType<T>();
    for (auto it = mapEventHandlers.begin(); it != mapEventHandlers.end(); )
    {
        if (it->first == id) mapEventHandlers.erase(it);
        else ++it;
    }
    EventBus::getInstance().unsubscribeEvent(id, shared_from_this());
}

template< typename T, class D >
void startTimer(const D& rel_time)
{
    EventId_t id = Event::getIdOfType<T>();
    auto it = mapTimers.find(id);
    if (it != mapTimers.end())
    {
        it->second.get()->startTimer(rel_time);
    }    
}

template< typename T, class D >
void startTimerPeriodic(const D& rel_time)
{
    EventId_t id = Event::getIdOfType<T>();
    auto it = mapTimers.find(id);
    if (it != mapTimers.end())
    {
        it->second.get()->startTimerPeriodic(rel_time);
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

void stopAllTimers(void);

private:

std::atomic_bool threadValid {true};
std::atomic_bool threadRunning {false};
std::queue<std::shared_ptr<Event>> evtQueue;
std::map<EventId_t, std::function<void(std::shared_ptr<Event>)>> mapEventHandlers;
std::map<EventId_t, std::unique_ptr<Timer>> mapTimers;
std::thread actorThread;
std::mutex mtxEventQueue;
std::mutex mtxEventLoop;
std::mutex mtxWaitShutdown;
std::condition_variable cvEventLoop;
std::condition_variable cvWaitShutdown;

static void eventLoopLauncher(Actor* p);
void eventLoop(void);
void finalizeAllTimers(void);
void handleEvent(std::shared_ptr<EventStartActor> pevt);
void handleEvent(std::shared_ptr<EventPauseActor> pevt);

};


} // namespace feal

#endif // _FEAL_ACTOR_H
