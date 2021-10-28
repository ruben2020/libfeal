// Please see the "examples/LICENSE" file for
// the copyright notice and license text.

#include <cstdio>
#include "feal.h"
#include "ActorA.h"

feal::EventId_t EventTimerA1::getId(void)
{
    return getIdOfType<EventTimerA1>();
}

void ActorA::initActor(void)
{
    printf("ActorA initActor\n");
    timers.init(this);
}

void ActorA::startActor(void)
{
    printf("ActorA startActor\n");

    timers.startTimer<EventTimerA1>(std::chrono::seconds(2), std::chrono::seconds(2));
}

void ActorA::pauseActor(void)
{
    printf("ActorA pauseActor\n");
}

void ActorA::shutdownActor(void)
{
    printf("ActorA shutdownActor\n");
}

void ActorA::handleEvent(std::shared_ptr<EventTimerA1> pevt)
{
    if (!pevt) return;
    printf("ActorA::TimerA1 Elapsed - press Ctrl-C to end program\n");
}


