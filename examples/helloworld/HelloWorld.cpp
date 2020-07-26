#include <stdio.h>
#include "HelloWorld.h"
#include "ActorA.h"
#include "ActorB.h"

feal::EventId_t EventTimerShutdown::getId(void)
{
    return getIdOfType<EventTimerShutdown>();
}

void HelloWorld::initActor(void)
{
    printf("HelloWorld initActor\n");
    actors.push_back(std::shared_ptr<Actor>(new ActorA()));
    actors.push_back(std::shared_ptr<Actor>(new ActorB()));
    feal::initAll(actors);
    registerTimer<HelloWorld, EventTimerShutdown>(this);
}

void HelloWorld::startActor(void)
{
    printf("HelloWorld startActor\n");
    feal::startAll(actors);
    startTimer<EventTimerShutdown>(std::chrono::seconds(23));
}

void HelloWorld::pauseActor(void)
{
    printf("HelloWorld pauseActor\n");
    feal::pauseAll(actors);
}

void HelloWorld::shutdownActor(void)
{
    printf("HelloWorld shutdownActor\n");
    feal::EventBus::getInstance().stopBus();
    feal::shutdownAll(actors);
    actors.clear();
    printf("HelloWorld shutdown complete\n");
}

void HelloWorld::handleEvent(std::shared_ptr<EventTimerShutdown> pevt)
{
    if (!pevt ) return;
    printf("HelloWorld::EventTimerShutdown Elapsed\n");
    shutdown();
}

int main(void)
{
    std::shared_ptr<HelloWorld> hworld (new HelloWorld());
    hworld.get()->init();
    hworld.get()->start();
    hworld.get()->wait_for_shutdown();
    printf("HelloWorld terminating program\n");
    return 0;
}
