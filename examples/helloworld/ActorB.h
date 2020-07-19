#ifndef _FEAL_ACTOR_B_H
#define _FEAL_ACTOR_B_H

#include "feal.h"

class EvtNotifyVege;

class ActorB : public feal::Actor
{

public:

ActorB() = default;
~ActorB() = default;


virtual void initActor(void);
virtual void startActor(void);
virtual void pauseActor(void);
virtual void shutdownActor(void);

void handleEvent(EvtNotifyVege& evt);

private:

};


#endif // _FEAL_ACTOR_B_H
