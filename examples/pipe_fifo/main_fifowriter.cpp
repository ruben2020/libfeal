//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <cstdio>

#include "fifowriter.h"

int main(void)
{
    printf("Pipe fifo writer starting program\n");
    std::shared_ptr<feal::Actor> actor = std::make_shared<Fifowriter>();
    actor.get()->init();
    actor.get()->start();
    actor.get()->waitForShutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("Pipe fifo writer terminating program\n");
    return 0;
}
