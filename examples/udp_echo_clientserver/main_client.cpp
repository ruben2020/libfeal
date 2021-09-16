#include <stdio.h>
#include "client.h"

int main(void)
{
    printf("udpclient starting program\n");
    std::shared_ptr<feal::Actor> tcpclient1 = std::make_shared<Client>();
    tcpclient1.get()->init();
    tcpclient1.get()->start();
    tcpclient1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("udpclient terminating program\n");
    return 0;
}


