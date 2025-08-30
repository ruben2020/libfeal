//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#ifndef FEAL_EVENT_H
#define FEAL_EVENT_H

#ifndef FEAL_H
#error "Please include feal.h and not the other internal Feal header files, to avoid include errors."
#endif

#include <vector>
#include <memory>

#define FEAL_EVENT_DEFAULT_DECLARE(X, Y)  \
    class X : public feal::Y              \
    {                                     \
       public:                            \
        X() = default;                    \
        X(const X&) = default;            \
        X& operator=(const X&) = default; \
        ~X() = default;                   \
    };

namespace feal
{

class Actor;

class Event : public std::enable_shared_from_this<Event>
{
    friend class Actor;

   public:
    Event() = default;
    Event(const Event&) = default;
    Event& operator=(const feal::Event&) = default;
    virtual ~Event() = default;

    std::shared_ptr<Event> getptr(void);

    void replyEvent(std::shared_ptr<Event> spevt);

    void setSender(std::weak_ptr<Actor>& act);

   private:
    std::weak_ptr<Actor> sender;
};

class EventComm : public Event
{
   public:
    EventComm() = default;
    EventComm(const EventComm&) = default;
    EventComm& operator=(const EventComm&) = default;
    ~EventComm() override = default;
    errenum_t errnum = FEAL_OK;
    handle_t fd = FEAL_INVALID_HANDLE;
    int datalen = -1;
    flags_t flags = 0;
};

}  // namespace feal

#endif  // FEAL_EVENT_H
