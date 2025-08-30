//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <cstdio>

#include "client6.h"

int main(void)
{
    printf("udpclient6 starting program\n");
    SOCK_STARTUP();
    std::shared_ptr<feal::Actor> tcpclient1 = std::make_shared<Client6>();
    tcpclient1.get()->init();
    tcpclient1.get()->start();
    tcpclient1.get()->waitForShutdown();
    feal::EventBus::getInstance().destroyInstance();
    SOCK_CLEANUP();
    printf("udpclient6 terminating program\n");
    return 0;
}
