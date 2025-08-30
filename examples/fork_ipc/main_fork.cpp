//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <cstdio>

#include "ActorA.h"
#include "ActorsManager.h"

int main(void)
{
    std::shared_ptr<feal::Actor> actmgr = std::make_shared<ActorsManager>();
    actmgr.get()->init();
    actmgr.get()->start();
    actmgr.get()->waitForShutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("Fork IPC example terminating program\n");
    return 0;
}
