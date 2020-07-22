#ifndef _FEAL_ACTOR_B_H
#define _FEAL_ACTOR_B_H

#include "feal.h"

class EvtNotifyVege;

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

private:

};


#endif // _FEAL_ACTOR_B_H
