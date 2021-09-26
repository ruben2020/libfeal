#include <cstdio>
#include "ActorsManager.h"
#include "ActorA.h"

feal::EventId_t EventTimerShutdown::getId(void)
{
    return getIdOfType<EventTimerShutdown>();
}

feal::EventId_t EvtSigInt::getId(void)
{
    return getIdOfType<EvtSigInt>();
}


void ActorsManager::initActor(void)
{
    printf("ActorsManager initActor\n");
    timers.init(this);
    signal.init(this);
    signal.registersignal<EvtSigInt>(SIGINT);
    actors.push_back(std::make_shared<ActorA>());
    feal::initAll(actors);
}

void ActorsManager::startActor(void)
{
    printf("ActorsManager startActor\n");
    feal::startAll(actors);
    timers.startTimer<EventTimerShutdown>(std::chrono::seconds(180));
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
    printf("Signal shutdown complete\n");
}

void ActorsManager::handleEvent(std::shared_ptr<EventTimerShutdown> pevt)
{
    if (!pevt ) return;
    printf("ActorsManager::EventTimerShutdown Elapsed\n");
    shutdown();
}

void ActorsManager::handleEvent(std::shared_ptr<EvtSigInt> pevt)
{
    if (!pevt ) return;
    printf("ActorsManager::EvtSigInt (signum=%d, sicode=%d)\n", 
        pevt.get()->signo, pevt.get()->sicode);
    timers.stopTimer<EventTimerShutdown>();
    shutdown();
}


