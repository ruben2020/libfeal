/*
 * Copyright 2021 ruben2020 https://github.com/ruben2020
 *
 * Dual-licensed under the Apache License, Version 2.0, and
 * the GNU General Public License, Version 2.0;
 * you may not use this file except in compliance
 * with either one of these licenses.
 *
 * You can find copies of these licenses in the included
 * LICENSE-APACHE2 and LICENSE-GPL2 files.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under these licenses is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the license texts for the specific language governing permissions
 * and limitations under the licenses.
 *
 * SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-only
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
