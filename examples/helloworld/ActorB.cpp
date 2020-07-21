#include <stdio.h>
#include "feal.h"
#include "ActorB.h"
#include "ActorA.h"

void ActorB::initActor(void)
{
    printf("ActorB initActor\n");
    subscribeEvent<ActorB, EvtNotifyVege>(this);
}

void ActorB::startActor(void)
{
    printf("ActorB startActor\n");
}

void ActorB::pauseActor(void)
{

}

void ActorB::shutdownActor(void)
{

}

void ActorB::handleEvent(EvtNotifyVege* pevt)
{
    printf("carrots = %d, tomatoes = %d\n", pevt->get_carrots(), pevt->get_tomatoes());
}

