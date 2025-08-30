//
// Copyright (c) 2022-2025 ruben2020 https://github.com/ruben2020
// SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
//

#include <cstdio>

#include "serverund.h"

int main(void)
{
    printf("Unix domain socket datagram server starting program\n");
    std::shared_ptr<feal::Actor> undserver1 = std::make_shared<Serverund>();
    undserver1.get()->init();
    undserver1.get()->start();
    undserver1.get()->waitForShutdown();
    feal::EventBus::getInstance().destroyInstance();
    printf("Unix domain socket datagram server terminating program\n");
    return 0;
}
