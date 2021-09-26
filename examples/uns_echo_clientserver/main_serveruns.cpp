#include <stdio.h>
#include "serveruns.h"

int main(void)
{
    printf("unix domain socket stream server starting program\n");
    std::shared_ptr<feal::Actor> unsserver1 = std::make_shared<Serveruns>();
    unsserver1.get()->init();
    unsserver1.get()->start();
    unsserver1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("unix domain socket stream server terminating program\n");
    return 0;
}

