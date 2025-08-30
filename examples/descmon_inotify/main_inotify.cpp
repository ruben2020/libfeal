//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <cstdio>

#include "ActorA.h"

int main(void)
{
    printf("inotifymon starting program\n");
    std::shared_ptr<feal::Actor> inotifymon = std::make_shared<ActorA>();
    inotifymon.get()->init();
    inotifymon.get()->start();
    inotifymon.get()->waitForShutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("inotifymon terminating program\n");
    return 0;
}
