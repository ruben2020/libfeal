#include <stdio.h>
#include "feal.h"
#include "ActorB.h"
#include "ActorA.h"

feal::EventId_t EventCabbage::getId(void)
{
    return getIdOfType<EventCabbage>();
}

void EventCabbage::set_cabbages(const int& num)
{
    cabbages = num;
}

int EventCabbage::get_cabbages(void)
{
    return cabbages;
}


feal::EventId_t EventTimerB1::getId(void)
{
    return getIdOfType<EventTimerB1>();
}

feal::EventId_t EventTimerB2::getId(void)
{
    return getIdOfType<EventTimerB2>();
}

void ActorB::initActor(void)
{
    printf("ActorB initActor\n");
    subscribeEvent<EvtNotifyVege>(this);
    subscribeEvent<EvtNotifyFruit>(this);
    registerTimer<EventTimerB1>(this);
    registerTimer<EventTimerB2>(this);
}

void ActorB::startActor(void)
{
    printf("ActorB startActor\n");
    startTimerPeriodic<EventTimerB1>(std::chrono::milliseconds(2000));
    startTimer<EventTimerB2>(std::chrono::seconds(11));
}

void ActorB::pauseActor(void)
{
    printf("ActorB pauseActor\n");
}

void ActorB::shutdownActor(void)
{
    printf("ActorB shutdownActor\n");
}

void ActorB::handleEvent(std::shared_ptr<EvtNotifyVege> pevt)
{
    if (pevt)
    printf("ActorB received carrots = %d, tomatoes = %d\n", pevt.get()->get_carrots(), pevt.get()->get_tomatoes());
}

void ActorB::handleEvent(std::shared_ptr<EvtNotifyFruit> pevt)
{
    if (pevt)
    printf("ActorB received apples = %d, oranges = %d\n", pevt.get()->get_apples(), pevt.get()->get_oranges());
}

void ActorB::handleEvent(std::shared_ptr<EventTimerB1> pevt)
{
    static int s_cabbages =0;
    if (!pevt) return;
    printf("ActorB::TimerB1 Elapsed\n");
    EventCabbage* evtCabbage = new EventCabbage();
    evtCabbage->set_cabbages(s_cabbages++);
    publishEvent(evtCabbage);

}


void ActorB::handleEvent(std::shared_ptr<EventTimerB2> pevt)
{
    if (!pevt) return;
    printf("ActorB::TimerB2 Elapsed\n");
    stopTimer<EventTimerB1>();
}
