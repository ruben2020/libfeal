//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
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

class Event : public std::enable_shared_from_this<Event>
{
friend class Actor;

public:

Event() = default;
Event( const Event & ) = default;
Event& operator= ( const feal::Event & ) = default;
virtual ~Event() = default;

std::shared_ptr<Event> getptr(void);

virtual EventId_t getId(void);
void replyEvent(std::shared_ptr<Event> spevt);

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

void setSender(std::weak_ptr<Actor>& act);

private:
std::weak_ptr<Actor> sender;
static EventId_t generateUniqueID(void);

};

class EventComm : public Event
{
public:
EventComm() = default;
EventComm( const EventComm & ) = default;
EventComm& operator= ( const EventComm & ) = default;
~EventComm() = default;
EventId_t getId(void);
errenum errnum = FEAL_OK;
handle_t fd = FEAL_INVALID_HANDLE;
int datalen = -1;
};


} // namespace feal
 
#endif // _FEAL_EVENT_H
