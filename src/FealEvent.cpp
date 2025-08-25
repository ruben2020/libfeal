//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include "feal.h"

std::shared_ptr<feal::Event> feal::Event::getptr(void)
{
    return shared_from_this();
}

void feal::Event::replyEvent(std::shared_ptr<Event> spevt)
{
    if ((sender.expired() == false) && (spevt))
    {
        std::shared_ptr<Actor> act = sender.lock();
        act.get()->receiveEvent(spevt);
    }
}

void feal::Event::setSender(std::weak_ptr<feal::Actor>& act)
{
    sender = act;
}
