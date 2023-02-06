//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#ifndef _FEAL_ACTORSMANAGER_H
#define _FEAL_ACTORSMANAGER_H

#include "feal.h"

class EvtAllDone : public feal::Event
{
public:
EvtAllDone() = default;
EvtAllDone( const EvtAllDone & ) = default;
EvtAllDone& operator= ( const EvtAllDone & ) = default;
~EvtAllDone() = default;
feal::EventId_t getId(void);
};

class ActorsManager : public feal::Actor
{

public:

ActorsManager() = default;
~ActorsManager() = default;


virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);

void handleEvent(std::shared_ptr<EvtAllDone> pevt);

private:
std::vector<std::shared_ptr<feal::Actor>> actors;

};


#endif // _FEAL_ACTORSMANAGER_H
