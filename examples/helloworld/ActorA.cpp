#include <stdio.h>
#include "feal.h"
#include "ActorA.h"
#include "ActorB.h"

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


feal::EventId_t EvtNotifyFruit::getId(void)
{
    return getIdOfType<EvtNotifyFruit>();
}

int EvtNotifyFruit::get_apples(void)
{
    return apples;
}

int EvtNotifyFruit::get_oranges(void)
{
    return oranges;
}
void EvtNotifyFruit::set_apples(const int& num)
{
    apples = num;
}
void EvtNotifyFruit::set_oranges(const int& num)
{
    oranges = num;
}

feal::EventId_t EventTimerA1::getId(void)
{
    return getIdOfType<EventTimerA1>();
}

void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
    subscribeEvent<ActorA, EventCabbage>(this);
    registerTimer<ActorA, EventTimerA1>(this);
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");

    startTimerPeriodic<EventTimerA1>(std::chrono::seconds(3));

    EvtNotifyVege* pevt = new EvtNotifyVege();
    pevt->set_carrots(10);
    pevt->set_tomatoes(20);
    publishEvent((feal::Event*) pevt);

    EvtNotifyFruit* pevt2 = new EvtNotifyFruit();
    pevt2->set_apples(30);
    pevt2->set_oranges(40);
    publishEvent((feal::Event*) pevt2);
}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EventCabbage> pevt)
{
    if (!pevt) return;
    printf("ActorA received cabbages = %d\n", pevt.get()->get_cabbages());

    auto pevt1 = new EvtNotifyVege();
    pevt1->set_carrots(100);
    pevt1->set_tomatoes(200);
    publishEvent((feal::Event*) pevt1);
    
    auto pevt2 = new EvtNotifyFruit();
    pevt2->set_apples(300);
    pevt2->set_oranges(400);
    publishEvent((feal::Event*) pevt2);
}


void ActorA::handleEvent(std::shared_ptr<EventTimerA1> pevt)
{
    if (!pevt) return;
    printf("ActorA::TimerA1 Elapsed\n");
}
