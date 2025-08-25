//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <stdio.h>

#include "client.h"

int main(void)
{
    printf("tlsclient starting program\n");
    SOCK_STARTUP();
    std::shared_ptr<feal::Actor> tcpclient1 = std::make_shared<Client>();
    tcpclient1.get()->init();
    tcpclient1.get()->start();
    tcpclient1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    SOCK_CLEANUP();
    printf("tlsclient terminating program\n");
    return 0;
}
