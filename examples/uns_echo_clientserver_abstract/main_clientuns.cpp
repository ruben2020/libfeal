//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <cstdio>

#include "clientuns.h"

int main(void)
{
    printf("unix domain socket stream client starting program\n");
    std::shared_ptr<feal::Actor> unsclient1 = std::make_shared<Clientuns>();
    unsclient1.get()->init();
    unsclient1.get()->start();
    unsclient1.get()->waitForShutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("unix domain socket stream client terminating program\n");
    return 0;
}
