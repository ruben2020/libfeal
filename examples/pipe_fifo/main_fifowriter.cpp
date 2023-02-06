//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: MIT
//
 
#include <stdio.h>
#include "fifowriter.h"

int main(void)
{
    printf("Pipe fifo writer starting program\n");
    std::shared_ptr<feal::Actor> actor = std::make_shared<Fifowriter>();
    actor.get()->init();
    actor.get()->start();
    actor.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("Pipe fifo writer terminating program\n");
    return 0;
}

