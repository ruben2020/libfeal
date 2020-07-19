#ifndef _FEAL_EVENT_BUS_H
#define _FEAL_EVENT_BUS_H

#include <memory>
#include <map>
#include <vector>
#include <mutex>

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
void subscribeEvent(EventId_t& evtid, Actor *ptr);
void unsubscribeEvent(EventId_t& evtid, Actor *ptr);
void publishEvent(Event& evt);
void resetBus(void);

private:

EventBus() = default;
static EventBus* inst;
map_evt_subsc_t mapEventSubscribers;
std::mutex mtxEventBus;

};


} // namespace feal

#endif // _FEAL_EVENT_BUS_H
