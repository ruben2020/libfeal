/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Licensed under the Apache License, Version 2.0
 * with LLVM Exceptions (the "License");
 * you may not use this file except in compliance with the License.
 * You can find a copy of the License in the LICENSE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 * 
 */
 
#include "feal.h"

feal::EventId_t feal::Event::getId(void)
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
