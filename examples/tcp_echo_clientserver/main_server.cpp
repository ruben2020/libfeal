//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <stdio.h>
#include "server.h"

int main(void)
{
    printf("tcpserver starting program\n");
    SOCK_STARTUP();
    std::shared_ptr<feal::Actor> tcpserver1 = std::make_shared<Server>();
    tcpserver1.get()->init();
    tcpserver1.get()->start();
    tcpserver1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    SOCK_CLEANUP();
    printf("tcpserver terminating program\n");
    return 0;
}

