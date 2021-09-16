#include <stdio.h>
#include "ActorsManager.h"
#include "ActorA.h"
#include "ActorB.h"

int main(void)
{
    std::shared_ptr<feal::Actor> actmgr = std::make_shared<ActorsManager>();
    actmgr.get()->init();
    actmgr.get()->start();
    actmgr.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("HelloWorld terminating program\n");
    return 0;
}
