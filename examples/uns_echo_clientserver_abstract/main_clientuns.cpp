//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <stdio.h>
#include "clientuns.h"

int main(void)
{
    printf("unix domain socket stream client starting program\n");
    std::shared_ptr<feal::Actor> unsclient1 = std::make_shared<Clientuns>();
    unsclient1.get()->init();
    unsclient1.get()->start();
    unsclient1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("unix domain socket stream client terminating program\n");
    return 0;
}

