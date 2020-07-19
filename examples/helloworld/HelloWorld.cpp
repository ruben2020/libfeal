#include <stdio.h>
#include "HelloWorld.h"
#include "ActorA.h"
#include "ActorB.h"

void HelloWorld::initActor(void)
{
    printf("HelloWorld initActor\n");
    actors.push_back(std::shared_ptr<Actor>(new ActorA()));
    actors.push_back(std::shared_ptr<Actor>(new ActorB()));
    feal::initAll(actors);
}

void HelloWorld::startActor(void)
{
    printf("HelloWorld initActor\n");
    feal::startAll(actors);
}

void HelloWorld::pauseActor(void)
{
    feal::pauseAll(actors);
}

void HelloWorld::shutdownActor(void)
{
    feal::shutdownAll(actors);
}



int main(void)
{
    HelloWorld hworld;
    hworld.init();
    hworld.start();
    hworld.wait_for_shutdown();
    return 0;
}
