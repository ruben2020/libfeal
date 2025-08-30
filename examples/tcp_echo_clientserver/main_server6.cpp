//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <cstdio>

#include "server6.h"

int main(void)
{
    printf("tcpserver6 starting program\n");
    SOCK_STARTUP();
    std::shared_ptr<feal::Actor> tcpserver1 = std::make_shared<Server6>();
    tcpserver1.get()->init();
    tcpserver1.get()->start();
    tcpserver1.get()->waitForShutdown();
    feal::EventBus::getInstance().destroyInstance();
    SOCK_CLEANUP();
    printf("tcpserver6 terminating program\n");
    return 0;
}
