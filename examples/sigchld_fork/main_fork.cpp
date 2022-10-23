//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <stdio.h>
#include "ActorsManager.h"
#include "ActorA.h"

int main(void)
{
    std::shared_ptr<feal::Actor> actmgr = std::make_shared<ActorsManager>();
    actmgr.get()->init();
    actmgr.get()->start();
    actmgr.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("Fork example terminating program\n");
    return 0;
}
