//
// Copyright (c) 2022 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
 
#include <stdio.h>
#include "fiforeader.h"

int main(void)
{
    printf("Pipe fifo reader starting program\n");
    std::shared_ptr<feal::Actor> actor = std::make_shared<Fiforeader>();
    actor.get()->init();
    actor.get()->start();
    actor.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("Pipe fifo reader terminating program\n");
    return 0;
}

