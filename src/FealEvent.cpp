//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include "feal.h"

feal::EventId_t feal::Event::getId(void)
{
    return 0; // which is invalid
}

feal::EventId_t feal::EventComm::getId(void)
{
    return 0; // which is invalid
}

feal::EventId_t feal::Event::generateUniqueID(void)
{
    static EventId_t uid = 0;
    ++uid;
    return uid;
}

std::shared_ptr<feal::Event> feal::Event::getptr(void)
{
    return shared_from_this();
}

void feal::Event::replyEvent(std::shared_ptr<Event> spevt)
{
    if ((sender.expired() == false)&&(spevt))
    {
        std::shared_ptr<Actor> act = sender.lock();
        act.get()->receiveEvent(spevt);
    }
}

void feal::Event::setSender(std::weak_ptr<feal::Actor>& act)
{
    sender = act;
}
