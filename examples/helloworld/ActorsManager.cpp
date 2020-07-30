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
    printf("HelloWorld initActor\n");
    actors.push_back(std::make_shared<ActorA>());
    actors.push_back(std::make_shared<ActorB>());
    feal::initAll(actors);
    registerTimer<EventTimerShutdown>(this);
}

void ActorsManager::startActor(void)
{
    printf("HelloWorld startActor\n");
    feal::startAll(actors);
    startTimer<EventTimerShutdown>(std::chrono::seconds(31));
}

void ActorsManager::pauseActor(void)
{
    printf("HelloWorld pauseActor\n");
    feal::pauseAll(actors);
}

void ActorsManager::shutdownActor(void)
{
    printf("HelloWorld shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    feal::shutdownAll(actors);
    actors.clear();
    printf("HelloWorld shutdown complete\n");
}

void ActorsManager::handleEvent(std::shared_ptr<EventTimerShutdown> pevt)
{
    if (!pevt ) return;
    printf("HelloWorld::EventTimerShutdown Elapsed\n");
    shutdown();
}
