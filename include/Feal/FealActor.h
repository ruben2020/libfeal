#ifndef _FEAL_ACTOR_H
#define _FEAL_ACTOR_H

#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <map>
#include <vector>

#include "FealEventBus.h"


namespace feal
{

class Event;

class Actor
{
public:

Actor() = default;
Actor(const Actor&) = default;
Actor& operator= ( const Actor & ) = default;
virtual ~Actor() = default;

void init(void);
void start(void);
void pause(void);
void shutdown(void);
void receiveEvent(Event& evt);
bool isActive(void);
void wait_for_shutdown(void);

protected:

virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);
void publishEvent(Event& evt);

template<typename Y, typename T>
void subscribeEvent(Y* p)
{
    void handleEvent(T&);
    EventId_t id = Event::getIdOfType<T>();
    mapEventHandlers.insert(std::make_pair(id,
            [p](Event& fe){  p->handleEvent(dynamic_cast<T&>(fe));  }
            )
        );
    EventBus::getInstance().subscribeEvent(id, this);
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
    EventBus::getInstance().unsubscribeEvent(id, this);
 }

private:

bool threadValid = true;
bool threadRunning = false;
std::queue<Event> evtQueue;
std::map<EventId_t, std::function<void(Event&)>> mapEventHandlers;
std::thread actorthread;
std::mutex mtxEventQueue;
std::mutex mtxEventLoop;
std::mutex mtxWaitShutdown;
std::condition_variable cvEventLoop;
std::condition_variable cvWaitShutdown;


static void eventLoop(Actor* p);

};

typedef  std::shared_ptr<Actor> actorptr_t;

typedef std::vector<actorptr_t> actor_vec_t;

void initAll(actor_vec_t& vec);
void startAll(actor_vec_t& vec);
void pauseAll(actor_vec_t &vec);
void shutdownAll(actor_vec_t& vec);
void receiveEventAll(actor_vec_t& vec, Event& evt);

} // namespace feal

#endif // _FEAL_ACTOR_H
