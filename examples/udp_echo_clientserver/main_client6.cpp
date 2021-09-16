#include <stdio.h>
#include "client6.h"

int main(void)
{
    printf("udpclient6 starting program\n");
    std::shared_ptr<feal::Actor> tcpclient1 = std::make_shared<Client6>();
    tcpclient1.get()->init();
    tcpclient1.get()->start();
    tcpclient1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("udpclient6 terminating program\n");
    return 0;
}


