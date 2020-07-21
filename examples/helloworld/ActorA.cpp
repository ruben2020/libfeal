#include <stdio.h>
#include "feal.h"
#include "ActorA.h"

feal::EventId_t EvtNotifyVege::getId(void)
{
    return getIdOfType<EvtNotifyVege>();
}

int EvtNotifyVege::get_carrots(void)
{
    return carrots;
}

int EvtNotifyVege::get_tomatoes(void)
{
    return tomatoes;
}
void EvtNotifyVege::set_carrots(const int& num)
{
    carrots = num;
}
void EvtNotifyVege::set_tomatoes(const int& num)
{
    tomatoes = num;
}


void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");
    EvtNotifyVege* pevt = new EvtNotifyVege();
    pevt->set_carrots(3);
    pevt->set_tomatoes(5);
    publishEvent((feal::Event*) pevt);
}

void ActorA::pauseActor(void)
{

}

void ActorA::shutdownActor(void)
{

}
