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
 
#ifndef _FEAL_ACTOR_B_H
#define _FEAL_ACTOR_B_H

#include "feal.h"

class EvtNotifyVege;
class EvtNotifyFruit;

class EventCabbage : public feal::Event
{
friend class ActorB;

public:
EventCabbage() = default;
EventCabbage( const EventCabbage & ) = default;
EventCabbage& operator= ( const EventCabbage & ) = default;
~EventCabbage() = default;
feal::EventId_t getId(void);

int get_cabbages(void);

private:
void set_cabbages(const int& num);
int cabbages=0;

};

class EventTimerB1 : public feal::Event
{
public:
EventTimerB1() = default;
EventTimerB1( const EventTimerB1 & ) = default;
EventTimerB1& operator= ( const EventTimerB1 & ) = default;
~EventTimerB1() = default;
feal::EventId_t getId(void);
};

class EventTimerB2 : public feal::Event
{
public:
EventTimerB2() = default;
EventTimerB2( const EventTimerB2 & ) = default;
EventTimerB2& operator= ( const EventTimerB2 & ) = default;
~EventTimerB2() = default;
feal::EventId_t getId(void);
};

class ActorB : public feal::Actor
{

public:

ActorB() = default;
~ActorB() = default;


void initActor(void);
void startActor(void);
void pauseActor(void);
void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtNotifyVege> pevt);
void handleEvent(std::shared_ptr<EvtNotifyFruit> pevt);
void handleEvent(std::shared_ptr<EventTimerB1> pevt);
void handleEvent(std::shared_ptr<EventTimerB2> pevt);

private:
feal::Timers<ActorB> timers;

};


#endif // _FEAL_ACTOR_B_H
