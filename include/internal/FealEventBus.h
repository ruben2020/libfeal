#ifndef _FEAL_EVENT_BUS_H
#define _FEAL_EVENT_BUS_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <memory>
#include <map>
#include <vector>
#include <mutex>
#include <atomic>

namespace feal
{

class Event;
class Actor;

typedef std::shared_ptr<Actor> actorptr_t;
typedef std::vector<actorptr_t> vec_actor_ptr_t;
typedef std::map<EventId_t, vec_actor_ptr_t> map_evt_subsc_t;

class EventBus
{

public:

EventBus( const EventBus & ) = delete;
EventBus& operator= ( const EventBus & ) = delete;
~EventBus();

static EventBus& getInstance(void);
static void destroyInstance(void);
void subscribeEvent(const EventId_t& evtid, actorptr_t ptr);
void unsubscribeEvent(const EventId_t& evtid, actorptr_t ptr);
void publishEvent(std::shared_ptr<Event> pevt);
void stopBus(void);
void resetBus(void);

private:

EventBus() = default;
static EventBus* inst;
map_evt_subsc_t mapEventSubscribers;
std::mutex mtxEventBus;
std::atomic_bool eventBusOff {false};

};


} // namespace feal

#endif // _FEAL_EVENT_BUS_H
