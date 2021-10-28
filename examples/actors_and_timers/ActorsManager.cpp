// Please see the "examples/LICENSE" file for
// the copyright notice and license text.

#include <stdio.h>
#include "ActorsManager.h"
#include "ActorA.h"
#include "ActorB.h"

feal::EventId_t EventTimerShutdown::getId(void)
{
    return getIdOfType<EventTimerShutdown>();
}

void ActorsManager::initActor(void)
{
    printf("ActorsManager initActor\n");
    timers.init(this);
    actors.push_back(std::make_shared<ActorA>());
    actors.push_back(std::make_shared<ActorB>());
    feal::initAll(actors);
}

void ActorsManager::startActor(void)
{
    printf("ActorsManager startActor\n");
    feal::startAll(actors);
    timers.startTimer<EventTimerShutdown>(std::chrono::seconds(31));
}

void ActorsManager::pauseActor(void)
{
    printf("ActorsManager pauseActor\n");
    feal::pauseAll(actors);
}

void ActorsManager::shutdownActor(void)
{
    printf("ActorsManager shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    feal::shutdownAll(actors);
    actors.clear();
    printf("Actors&Timers shutdown complete\n");
}

void ActorsManager::handleEvent(std::shared_ptr<EventTimerShutdown> pevt)
{
    if (!pevt ) return;
    printf("ActorsManager::EventTimerShutdown Elapsed\n");
    shutdown();
}
