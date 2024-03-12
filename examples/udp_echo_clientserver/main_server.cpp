//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <stdio.h>
#include "server.h"

int main(void)
{
    printf("udpserver starting program\n");
    SOCK_STARTUP();
    std::shared_ptr<feal::Actor> tcpserver1 = std::make_shared<Server>();
    tcpserver1.get()->init();
    tcpserver1.get()->start();
    tcpserver1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    SOCK_CLEANUP();
    printf("udpserver terminating program\n");
    return 0;
}

