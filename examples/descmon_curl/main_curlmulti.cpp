//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//
 
#include <stdio.h>
#include <curl/curl.h>
#include "ActorA.h"

int main(void)
{
    printf("curlmulti starting program\n");
    std::shared_ptr<feal::Actor> curlmulti1 = std::make_shared<ActorA>();
    curlmulti1.get()->init();
    curlmulti1.get()->start();
    curlmulti1.get()->wait_for_shutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("curlmulti terminating program\n");
    return 0;
}


