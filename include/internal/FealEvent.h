#ifndef _FEAL_EVENT_H
#define _FEAL_EVENT_H

#ifndef _FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <vector>
#include <memory>

namespace feal
{

class Actor;

class Event
{
friend class Actor;

public:

Event() = default;
Event( const Event & ) = default;
Event& operator= ( const feal::Event & ) = default;
virtual ~Event() = default;

virtual EventId_t getId(void);
void replyEvent(Event* pevt);

protected:

void setSender(feal::Actor* act);

template<typename T>
 static EventId_t getIdOfType()
 {
     static EventId_t s_evtId = 0;
     if ( s_evtId == 0 )
     {
        s_evtId = generateUniqueID();
     }
     return s_evtId;
 }


private:
std::shared_ptr<Actor>* sender = nullptr;
static EventId_t generateUniqueID(void);

};

} // namespace feal
 
#endif // _FEAL_EVENT_H
