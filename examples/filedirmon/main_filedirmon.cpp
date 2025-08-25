//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <stdio.h>

#include "ActorA.h"

int main(void)
{
    printf("filedirmon starting program\n");
    std::shared_ptr<feal::Actor> filedirmon1 = std::make_shared<ActorA>();
    filedirmon1.get()->init();
    filedirmon1.get()->start();
    filedirmon1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("filedirmon terminating program\n");
    return 0;
}
