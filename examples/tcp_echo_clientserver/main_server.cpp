#include <stdio.h>
#include "server.h"

int main(void)
{
    printf("tcpserver starting program\n");
    std::shared_ptr<feal::Actor> tcpserver1 = std::make_shared<Server>();
    tcpserver1.get()->init();
    tcpserver1.get()->start();
    tcpserver1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("tcpserver terminating program\n");
    return 0;
}

